/*
 * MonitorCTRL.cpp
 *
 *	Continuous sampling of all the PSUs (and auxiliaries, when properly set). To do so, the RTOS task
 *	waits for the interrupt to post on monitorSem (an OS_SEM), and then goes over the next loop.
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Controller/Controller.h"
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE sniList [INT_VCC_n12V + 1];				// Supply & Internal voltages List

//==============================================VARIABLES==============================================//
// Imported
extern OS_SEM monitorSem;					// Semaphore where PIT1 Posts and monitorTask Pends.
extern BOOL maxAGCReached;					// AGC boolean that determines if the maximum gain has been reached

// A2D Sampling
int lastMeasure = 0;						// Stores the A2D result
BOOL lastMeasureValid = false;				// MIN < lastMeasure < MAX
BYTE monitorSamplFunc = FUNCTION_PSU_VOLTAGE;// Current Sampling function. FUNCTION_PSU_VOLTAGE, FUNCTION_PSU_CURRENT, FUNCTION_SnI_VOLTAGE
int monitorNum = 0;							// Current PSU/SnI. 0 to 11 for PSUs, 0 to 13 for SnIs

// Configuration Flags
extern BOOL config_MonitorCTRL_SnI_FLAG;	// Sets whether both Supply and Internal voltages are included in a2d routine loop or not

// Testing
extern BOOL seqMode_MonitorCTRL_Task_FLAG;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem
extern OS_SEM seqMode_MonitorCTRLTask_Sem;
extern BOOL testMode_MonitorCTRL_Measure_FLAG;// Uses testMode_Measure instead of reading from ADC
extern WORD testMode_Measure;				// Used to simulate values. Recommended tu be used with VoltORCurrToADCCounts()

// Auxiliary
int a = 0;


//=====================================================================================================//
//===================================== PUBLIC MONITOR METHODS ========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// monitorTask - OS Task with the lowest priority (first in line of execution) in charge of a2d sampling
//				 and storage.
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.:	OSSimpleTaskCreate( monitorTask, MONITOR_PRIO );; - used in main();
//-------------------------------------------------------------------------------------------------------
void monitorTask (void* p){
	while(1){	// loop forever

		OSSemPend( & monitorSem , 0 );
		if (seqMode_MonitorCTRL_Task_FLAG){OSSemPend( & seqMode_MonitorCTRLTask_Sem , 0 );}

		lastMeasure = (testMode_MonitorCTRL_Measure_FLAG?testMode_Measure:(ReadA2DResult(0) >> 3));
		lastMeasureValid = ((lastMeasure>MINIMUM_LEVEL_ADC || maxAGCReached) &&(lastMeasure<MAXIMUM_LEVEL_ADC));
		if (lastMeasureValid){
			switch (monitorSamplFunc){	//Store Value in the right object
				case FUNCTION_PSU_VOLTAGE:
					psuList[monitorNum].vOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum); break;
				case FUNCTION_PSU_CURRENT:
					psuList[monitorNum].cOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum);	break;
				case FUNCTION_SnI_VOLTAGE:
					sniList[monitorNum].vOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum);	break;
			}

			minAGC();	// Set AGC always to minimum upon next voltage sampling

			// Next PSU, and next samplingFunction (see defineConstants.cpp - MUX) if last PSU reached
			monitorNum++;
			if (config_MonitorCTRL_SnI_FLAG){ // SnI included in
				if( ((monitorSamplFunc<=FUNCTION_PSU_CURRENT) &&  (monitorNum>=PSU_NUMBER)) || (monitorNum>=SnI_NUMBER) ){
					monitorNum = 0;
					// Next sampling function (restart if last function reached, cyclic)
					monitorSamplFunc = (monitorSamplFunc>=FUNCTION_SnI_VOLTAGE?FUNCTION_PSU_VOLTAGE:monitorSamplFunc+1);
				}
			}
			else{
				if (monitorNum >= PSU_NUMBER){
					monitorNum = 0;
					monitorSamplFunc = !monitorSamplFunc; 	// Toggle between FUNCTION_PSU_VOLTAGE (0) and FUNCTION_PSU_CURRENT(1)
				}										  	//  functions (see defineConstants.cpp - MUX)
			}
			setMUX( monitorSamplFunc, (BYTE)monitorNum );	// Set muxes for next reading
		}
		else{
			if (lastMeasure!=0)	{scaleGainAGC(MIDSCALE_ADC/lastMeasure);}
			else				{scaleGainAGC(3);}
		}
	}
}
