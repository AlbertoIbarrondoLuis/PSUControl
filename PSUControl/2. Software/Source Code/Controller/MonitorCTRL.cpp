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
#include "Interface/Interface.h"			// Exceptionally, for method  valuesDisplayINFC

extern PSU_TYPE psuList[PSU_NUMBER];		// MAIN PSU ARRAY LIST
extern SnI_TYPE supList [SUP_NUMBER];		// Supply & Internal voltages List

//==============================================VARIABLES==============================================//
// Imported
extern OS_SEM monitorSem;					// Semaphore where PIT1 Posts and monitorTask Pends.
extern BOOL maxAGCReached;					// AGC boolean that determines if the maximum gain has been reached

// A2D Sampling
int lastMeasure = 0;						// Stores the A2D result
BOOL lastMeasureValid = false;				// MIN < lastMeasure < MAX
int measureCounts = 0;						// Number of measures before storing value. Set to 0 when stored
BYTE monitorSamplFunc = FUNCTION_PSU_VOLTAGE;// Current Sampling function. FUNCTION_PSU_VOLTAGE, FUNCTION_PSU_CURRENT, FUNCTION_SnI_VOLTAGE
int monitorNum = 0;							// Current PSU/SnI. 0 to 11 for PSUs, 0 to 13 for SnIs

// Configuration Flags
extern BOOL config_MonitorCTRL_SnI_FLAG;	// Sets whether both Supply and Internal voltages are included in a2d routine loop or not
extern BOOL config_MonitorCTRL_PrintVals;	// Prints values periodically
extern BOOL monitor_print_FLAG;				// Flag meaning a print is ready

// Testing
extern BOOL seqMode_MonitorCTRL_Task_FLAG;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem
extern OS_SEM seqMode_MonitorCTRLTask_Sem;
extern BOOL testMode_MonitorCTRL_Measure_FLAG;// Uses testMode_Measure instead of reading from ADC
extern BOOL testMode_MonitorCTRL_Print_FLAG;// Prints value when valid
extern WORD testMode_Measure;				// Used to simulate values. Recommended to be used with VoltORCurrToADCCounts()
float valueStored = 0;

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
		if (monitor_print_FLAG && config_MonitorCTRL_PrintVals){
			valuesDisplayINFC ();
			monitor_print_FLAG = false;
		}

		// Measure Loop
		if (seqMode_MonitorCTRL_Task_FLAG){OSSemPend( & seqMode_MonitorCTRLTask_Sem , 0 );}
		OSSemPend( & monitorSem , 0 );


		lastMeasure = (testMode_MonitorCTRL_Measure_FLAG?testMode_Measure:(ReadA2DResult(0) >> 3));
		measureCounts++;
		lastMeasureValid = ((lastMeasure>MINIMUM_LEVEL_ADC || maxAGCReached) &&(lastMeasure<MAXIMUM_LEVEL_ADC));
		if (testMode_MonitorCTRL_Print_FLAG || seqMode_MonitorCTRL_Task_FLAG){
				iprintf(" ~ ADC: 0x%x  (%s)  |   ",lastMeasure, (lastMeasureValid?"OK":""));
			}
		if (lastMeasureValid || measureCounts>3){
			measureCounts = 0;
			switch (monitorSamplFunc){	//Store Value in the right object
				case FUNCTION_PSU_VOLTAGE:
					psuList[monitorNum].vOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum);
					valueStored = psuList[monitorNum].vOut;
					psuList[monitorNum].vMean = psuList[monitorNum].vMean * 0.5 + psuList[monitorNum].vOut * 0.5;
					break;
				case FUNCTION_PSU_CURRENT:
					psuList[monitorNum].cOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum) - psuList[monitorNum].iOffset;
					valueStored = psuList[monitorNum].cOut;
					psuList[monitorNum].vMean = psuList[monitorNum].cMean * 0.5 + psuList[monitorNum].cOut * 0.5;
					break;
				case FUNCTION_SnI_VOLTAGE:
					supList[monitorNum].vOut= ADCCountsToVoltORCurr(lastMeasure, monitorSamplFunc, monitorNum);
					valueStored = supList[monitorNum].vOut;					break;
			}

			if (testMode_MonitorCTRL_Print_FLAG || seqMode_MonitorCTRL_Task_FLAG){
				iprintf("~PSU %d (%s): %s\n", monitorNum, (monitorSamplFunc?"i":"v"), ftos(valueStored, 2));
			}

			minAGC();	// Set AGC always to minimum upon next voltage sampling

			// Next PSU, and next samplingFunction (see defineConstants.cpp - MUX) if last PSU reached
			monitorNum++;
			if (config_MonitorCTRL_SnI_FLAG){ // SnI included in
				if( ((monitorSamplFunc<=FUNCTION_PSU_CURRENT) &&  (monitorNum>=PSU_NUMBER)) || (monitorNum>=SUP_NUMBER) ){
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



//-------------------------------------------------------------------------------------------------------
// resetMonitor - Sets MonitorTask into initial status, adjusting the Muxes for first reading and setting
//					the AGC into minimum gain
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.:	resetMonitor();
//-------------------------------------------------------------------------------------------------------
void resetMonitor ( void ){
	seqMode_MonitorCTRL_Task_FLAG=ON;		//Stop MonitorTask

	minAGC();								// Set AGC to minimum

	monitorNum=0;							// Muxes set for first reading
	monitorSamplFunc=FUNCTION_PSU_VOLTAGE;
	setMUX( monitorSamplFunc, (BYTE)monitorNum );
}

