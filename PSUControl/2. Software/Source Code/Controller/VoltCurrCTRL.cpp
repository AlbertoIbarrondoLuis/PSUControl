/*
 * VoltCurrCTRL.cpp
 *
 *	Change output voltage for PSUs, single reads of PSUs and SnIs (both voltage and current).
 *	Conversion between A2D counts and Voltage/Current Values
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Controller/Controller.h"


//===========================================VARIABLES=================================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];		// MAIN PSU ARRAY LIST
extern SnI_TYPE supList [SUP_NUMBER];		// Supply & Internal voltages List

// Scale Factor for ADC readings, accessible with _sf(samplingFunction, Num)
float scaleFactorArray[37] = SCALE_FACTOR_ARRAY;

// Pause Tasks
extern BOOL idleMode_AlarmCTRL_Task_FLAG;		// Sets Alarm Task into idle mode
extern BOOL seqMode_MonitorCTRL_Task_FLAG;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem
BOOL idleModeAlarmTask_VoltCurr, seqModeMonitorTask_VoltCurr;

//=====================================================================================================//
//=============================== PUBLIC VOLTAGE & CURRENT METHODS ====================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// adjustRdac - changes the RDAC Register Value (thus modifying its output voltage)
// 					Checks for correct value updating
// 		#INPUT: int psuNum - PSU number whose RDAC will be updated
//				float Voltage - Value in volts to be programmed. 1.26 to 32
// 		#OUTPUT: BOOL programSuccess - Value has been successfully programmed
//		>E.G.: adjustRdac( 2, 5.14 );
//-------------------------------------------------------------------------------------------------------
float adjustRdac (int psuNum, float Voltage){
	BOOL isNegative = psuNum>SF5_B; int desiredValue = 1023;
	if(psuNum<0 || psuNum>SF6_B){iprintf("ERROR - PSU number out of bounds\n");	return 0;}

	//Checking that Voltage isn't out of bounds and converting to counts
	if   (isNegative)	 {
		if (Voltage>-2.6)	{ Voltage = -2.6;		}
		if (Voltage<-16)	{ Voltage = -16;		}
		desiredValue = voltNegativeToCounts(Voltage, psuList[psuNum].rShunt);}
	else /*is Positive*/ {
		if (Voltage>32)		{ Voltage = 32;			}
		if (Voltage<1.25)	{ Voltage = 1.25;		}
		desiredValue = voltPositiveToCounts(Voltage, psuList[psuNum].rShunt);}
	return adjustRdac (psuNum, desiredValue, Voltage);
}

float adjustRdac (int psuNum, int rdacCounts, float Voltage/* not used, just for differentiation*/){
	BOOL isNegative = psuNum>SF5_B; float Volt;

	setValRDAC(rdacCounts, psuList[psuNum].rdacAdr, psuList[psuNum].bridgeI2CAdr);
	iprintf(" \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b \b");
	int setValue = getValRDAC(psuList[psuNum].rdacAdr, psuList[psuNum].bridgeI2CAdr);

	if (rdacCounts==setValue){ // Correctly programmed. Convert counts to volts and save in psuList
		Volt = (isNegative? 	countsToVoltNegative(rdacCounts, psuList[psuNum].rShunt) : \
								countsToVoltPositive(rdacCounts, psuList[psuNum].rShunt));
		psuList[psuNum].progVolt = Volt;
		psuList[psuNum].progCounts = rdacCounts;
		return Volt;
	}else{						 // Error of some kind
		return 0;
	}
}


//-------------------------------------------------------------------------------------------------------
// resetRdacs - Hardware rheostats' reset using the corresponding Pin (21)
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: resetRdacs();
//-------------------------------------------------------------------------------------------------------
void resetRdacs ( void ){
	RESET_RHEOSTATS = 1;
	OSTimeDly(TICKS_100MS);
	RESET_RHEOSTATS = 0;
}


//-------------------------------------------------------------------------------------------------------
// updateVoltagePSUs - Updates both RDAC values with their RAM configuration for the selected PSUs
//					  (marking its respective bit number as TRUE)
// 		#INPUT: BOOL psuSelection[PSU_NUMBER] - boolean array with TRUE booleans being PSUs to be updated
// 		#OUTPUT: None
//		>E.G.: updateVoltagePSUs(spuSelectionList);
//-------------------------------------------------------------------------------------------------------
void updateVoltagePSUs( BOOL psuSelection[PSU_NUMBER] ){	// PSUs marked with his corresponding bit to 1 will be switched on.
	int psuNum;
	// ajustar fuentes
	for (psuNum=0;psuNum<PSU_NUMBER;psuNum++){
		if(psuSelection[psuNum]){if (psuList[psuNum].psuStatus) {
				adjustRdac (psuNum, psuList[psuNum].progVolt);
		}}
 	}
	OSTimeDly(TICKS_100MS); // delay for regulator adjustment
}


//-------------------------------------------------------------------------------------------------------
// readVoltageValue - Reads a sample of the ADC by properly configuring the Muxes and converts it
//						to voltage.  To do so, it stops monitorTask() (which does it automatically)
// 		#INPUT: int Num - PSU/SnI number whose Voltage will be read
//				BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
// 		#OUTPUT: None
//		>E.G.: readVoltageValue(2, PSU_TYPE_LIST);
//-------------------------------------------------------------------------------------------------------
void readVoltageValue(int Num, BOOL psu_sni){
	float value = 0; WORD a2dRes=0;
	BOOL seqMode_monitor = seqMode_MonitorCTRL_Task_FLAG;
	BYTE SamplFunc = (psu_sni==PSU_TYPE_LIST?FUNCTION_PSU_VOLTAGE:FUNCTION_SnI_VOLTAGE);

	if (!seqMode_monitor){sequentialMode_MonitorCTRL_Task(ON);} // Stops monitorTask

	setMUX(SamplFunc, Num);	OSTimeDly(1);

	a2dRes = (ReadA2DResult(0) >> 3);
	value = ADCCountsToVoltORCurr(a2dRes, SamplFunc, Num);
	iprintf(" Current Reading (%s %d): %s A (0x%x in A2D)\n", (psu_sni==PSU_TYPE_LIST?"PSU":"SnI"), Num, ftos(value), a2dRes);
	if (psu_sni == PSU_TYPE_LIST){	psuList[Num].vOut = value;}
	else{							supList[Num].vOut = value;}
	if (!seqMode_monitor){sequentialMode_MonitorCTRL_Task(OFF);} // Restarts monitorTask

}


//-------------------------------------------------------------------------------------------------------
// readCurrentValue - Reads a sample of the ADC by properly configuring the Muxes and converts it
//						to current. To do so, it stops monitorTask() (which does it automatically)
// 		#INPUT: int psuNum - PSU number whose Current will be read
// 		#OUTPUT: None
//		>E.G.: readCurrentValue(2, PSU_TYPE_LIST);
//-------------------------------------------------------------------------------------------------------
void readCurrentValue(int psuNum){
	float value = 0; WORD a2dRes=0;
	BOOL seqMode_monitor = seqMode_MonitorCTRL_Task_FLAG;

	if (!seqMode_monitor){sequentialMode_MonitorCTRL_Task(ON);} // Stops monitorTask

	setMUX(FUNCTION_PSU_CURRENT, psuNum);	OSTimeDly(1);

	a2dRes = (ReadA2DResult(0) >> 3);
	value = ADCCountsToVoltORCurr(a2dRes, FUNCTION_PSU_CURRENT, psuNum);
	iprintf(" Current Reading (PSU %d): %s A (0x%x in A2D)\n", psuNum, ftos(value), a2dRes);
	psuList[psuNum].cOut = value;
	if (!seqMode_monitor){sequentialMode_MonitorCTRL_Task(OFF);} // Restarts monitorTask
}


//-------------------------------------------------------------------------------------------------------
// resetMeasuresAll - Sets all vOut and cOut of psuList and supList to 0 (variables where ADC measures
//						are stored)
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: resetMeasuresAll(); - used in TEST_MonitorCTRL_NOMINALVALUES()
//-------------------------------------------------------------------------------------------------------
void resetMeasuresAll ( void ){
	int i;
	for (i=0; i<PSU_NUMBER; i++){psuList[i].vOut = 0;psuList[i].cOut = 0;}
	for (i=0; i<SUP_NUMBER; i++){supList[i].vOut = 0;}
}


//-------------------------------------------------------------------------------------------------------
// VoltORCurrToADCCounts - Conversion from real value (voltage or current) to ADC 12-bit WORD value.
//							Dependent on the AGC gain.
// 		#INPUT: float value - Voltage/Current value
//				int samplingFunction - FUNCTION_PSU_VOLTAGE/FUNCTION_PSU_CURRENT/FUNCTION_SnI_VOLTAGE
//				int Num - 0 to 11 for PSUs, 0 to 13 for SnIs
// 		#OUTPUT: WORD ADCCounts - ADC counts (12 bits)
//		>E.G.: VoltORCurrToADCCounts(16.5, FUNCTION_SnI_VOLTAGE, 4);
//-------------------------------------------------------------------------------------------------------
WORD VoltORCurrToADCCounts ( float value, int samplingFunction, int Num ){
	return (WORD)(abs(round((value*getGainAGC()*(4095.0))/(SCALE_FACTOR(samplingFunction, Num)*3.3))));
}


//-------------------------------------------------------------------------------------------------------
// ADCCountsToVoltORCurr - Conversion from ADC 12-bit WORD value to real value (voltage or current)
//							Dependent on the AGC gain.
// 		#INPUT: WORD ADCCounts - ADC counts (12 bits)
//				int samplingFunction - FUNCTION_PSU_VOLTAGE/FUNCTION_PSU_CURRENT/FUNCTION_SnI_VOLTAGE
//				int Num - 0 to 11 for PSUs, 0 to 13 for SnIs
// 		#OUTPUT: float value - Voltage/Current value
//		>E.G.: ADCCountsToVoltORCurr( 0x745, FUNCTION_PSU_CURRENT, 4);
//-------------------------------------------------------------------------------------------------------
float ADCCountsToVoltORCurr ( WORD ADCCounts, int samplingFunction, int Num ){
	return (((float)ADCCounts) * 3.3 * SCALE_FACTOR(samplingFunction, Num) / (getGainAGC()* (4095.0)));
}
