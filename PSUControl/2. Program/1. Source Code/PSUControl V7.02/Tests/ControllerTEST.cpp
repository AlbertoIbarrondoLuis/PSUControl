/*
 * Test_Controller.cpp
 *
 *	Software test for each of the Controller Modules
 *
 *  Created on: 20/05/2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Tests/Tests.h"


//==============================================VARIABLES==============================================//
// Keyboard
extern char testChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD testChar = sgetchar(0);
extern char categorySelectionTEST;

// Imported Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE sniList [INT_VCC_n12V + 1];				// Supply & Internal voltages List

//---------Imported from MonitorCTRL.cpp
// Variables
extern BYTE monitorSamplFunc;							// FUNCTION_PSU_VOLTAGE, FUNCTION_PSU_CURRENT, FUNCTION_SnI_VOLTAGE
extern int monitorNum;									// 0 to 11 for PSUs, 0 to 13 for SnIs
extern int testMode_psu_initializationTimer[PSU_NUMBER];// Records the real initializaionTimer values for the PSUs in switchONTask()

//---------Imported from generalTEST.cpp
// Results
extern BYTE results[15];
extern BYTE resultTotal;

// Auxiliary
int test_Num = 0, storedNum;
WORD test_measure = 0x800;

//=====================================================================================================//
//================================    Controller Testing METHODS    ===================================//
//=====================================================================================================//


//-------------------------------------------   MENU   -----------------------------------------------//
void ControllerTEST ( void ){
	ERASE_CONSOLE
	iprintf("============================== CONTROLLER TEST MENU ============================\r\n" );
	iprintf( "\nSOFTWARE TESTS\r\n" );
	iprintf( " (1) TEST_TIMER_INTERRUPTS\r\n" );
	iprintf( " (2) TEST_AlarmCTRL \r\n" );
	iprintf( " (3) TEST_DataListsCTRL \r\n" );
	iprintf( " (4) TEST_FlashMemCTRL \r\n" );
	iprintf( " (5) TEST_MonitorCTRL \r\n" );
	iprintf( " (6) TEST_SwitchOnCTRL \r\n" );
	iprintf( " (7) TEST_VoltCurrCTRL \r\n" );
	iprintf( "\n (e) EXIT TO GENERAL MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nEnter command: " );
	categorySelectionTEST = sgetchar( 0 ); iprintf ("%c\n", categorySelectionTEST);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
	switch ( categorySelectionTEST ){
		case '1': 	TEST_Timer_Interruption();		break;
		case '2':	TEST_AlarmCTRL();		break;
		case '3':	TEST_DataListsCTRL();	break;
		case '4':	TEST_FlashMemCTRL();	break;
		case '5':	TEST_MonitorCTRL();		break;
		case '6':	TEST_SwitchOnCTRL();	break;
		case '7':	TEST_VoltCurrCTRL();	break;
		case EXIT: 	iprintf( "\nGOING BACK TO TEST MENU\n");	categorySelectionTEST = EXIT;	break;
		default:	iprintf( "\nINVALID COMMAND -> %c\r\n", categorySelectionTEST);				break;
	}
	if (categorySelectionTEST!=EXIT){WAIT_FOR_KEYBOARD}
}


//---------------------------------------   GLOBAL TESTS    -------------------------------------------//

void TEST_Timer_Interruption ( void ){
	iprintf("\n\n\n\n(a). TEST_TIMER_INTERRUPTS (press e to exit)\n");
	while (testChar!='e'){
		WAIT_FOR_KEYBOARD
		iprintf("Pitr count zero = %ld Pitr count one = %ld\r\n", pitr_count_adc_sampling, pitr_count_zero);
	}
}

BOOL TEST_AlarmCTRL( void ){
	sequentialMode_MonitorCTRL_Task(ON);
	iddleMode_AlarmCTRL_Task(ON);
	iprintf("\n\n\n\n===============TEST_AlarmCTRL===============\n");
	//PSU
	TEST_AlarmCTRL_PSU1();
	TEST_AlarmCTRL_PSU2();
	TEST_AlarmCTRL_PSU3();
	TEST_AlarmCTRL_PSU4();

	//SnI
	TEST_AlarmCTRL_SnI1();
	TEST_AlarmCTRL_SnI2();
	TEST_AlarmCTRL_SnI3();

	//Results
	iprintf("\n\n\n\nTEST_AlarmCTRL\n");
	iprintf("\n PART 1 - PSU Inferior Voltage Alarm Triggered with no time\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 1 - PSU No false alarms \n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - PSU Superior Voltage Alarm Triggering with 3 times. \n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - PSU No false alarms\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - PSU Inferior Current Alarm Triggering with 2 times and Disconnection.\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - PSU No false alarms\n");
	iprintf(" ~results[5]: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - PSU Multiple Alarm Triggering for Upper Current\n");
	iprintf(" ~results[6]: %s\n", (results[6]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - SnI Inferior Voltage Alarm Triggering with no time. \n");
	iprintf(" ~results[7]: %s\n", (results[7]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 6 - SnI Superior Voltage Alarm Triggering with 3/2 times. \n");
	iprintf(" ~results[8]: %s\n", (results[8]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 7 - SnI Multiple Alarm Triggering for Upper Voltage\n");
	iprintf(" ~results[9]: %s\n", (results[9]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]&&results[6]&&results[7]&&results[8]&&results[9]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	iddleMode_AlarmCTRL_Task(OFF);
	sequentialMode_MonitorCTRL_Task(OFF);
	return resultTotal;
}






BOOL TEST_DataListsCTRL( void ){
	iprintf("\n\n\n\n===============TEST_DataListsCTRL===============\n");
	iprintf("\n\n\n\n1. Setting DEFAULT Values for PSUs 1, 4 (both positive) and 10 (negative)\n");
	defaultValuesPSU(1);
	defaultValuesPSU(4);
	defaultValuesPSU(11);
	printValuesPSU(1);
	printValuesPSU(4);
	printValuesPSU(11);
	//Check Values
	results[0] = (getPSU(4).alarmProtocolShutdown[0] == demux4to16(4+1));
	results[1] = (getPSU(1).alarmProtocolShutdown[0] == demux4to16(1+1));
	results[2] = (getPSU(11).alarmProtocolShutdown[0] == demux4to16(11+1));
	//end Check Values
	iprintf(" Saved Value (1) = %d (should be %d)\n", getPSU(1).alarmProtocolShutdown[0], demux4to16(1+1));
	iprintf(" Saved Value (4) = %d (should be %d)\n", getPSU(4).alarmProtocolShutdown[0], demux4to16(4+1));
	iprintf("\n\n\n\n2. Setting DEFAULT Values for SnIs 1 and 4\n");
	defaultValuesSnI(1);
	defaultValuesSnI(4);
	defaultValuesSnI(13);
	printValuesSnI(1);
	printValuesSnI(4);
	printValuesSnI(13);
	//Check Values
	results[3] = ((getSnI(4).alarmLimitValues[0] < getSnI(4).nominalVolt) && (getSnI(4).alarmLimitValues[1] > getSnI(4).nominalVolt));
	results[4] = ((getSnI(1).alarmLimitValues[0] < getSnI(1).nominalVolt) && (getSnI(1).alarmLimitValues[1] > getSnI(1).nominalVolt));
	results[5] = ((getSnI(13).alarmLimitValues[0] < getSnI(13).nominalVolt) && (getSnI(13).alarmLimitValues[1] > getSnI(13).nominalVolt));
	// end Check Values
	iprintf("\n\n\n\n\n TEST_DataListsCTRL\n");
	iprintf("\n PART 1 - Setting default Values to PSUs\n");
	iprintf(" ~results[0](PSU 4): %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf(" ~results[1](PSU 1): %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf(" ~results[2](PSU 11): %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Setting default Values to SnIs\n");
	iprintf(" ~results[3](SnI 4): %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf(" ~results[4](SnI 1): %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf(" ~results[5](SnI 13): %s\n", (results[5]?"PASSED":"NOT PASSED"));
	results[6] = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]);
	iprintf("\n OVERALL RESULT: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	return results[5];
}





BOOL TEST_FlashMemCTRL( void ){
	// NOTE: all the printValues() methods are commented. In case of any error, toggle comment to analyze it.
	iprintf("\n\n\n\n===============TEST_FlashMemCTRL===============\n");

	//PSU
	TEST_FlashMemCTRL_PSU();

	//SnI
	TEST_FlashMemCTRL_SnI();

	//Results
	iprintf("\n TEST_FlashMemCTRL\n");
	iprintf("\n PART 1 - Setting default Values and saving values for PSUs\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Restore Saved Values for PSUs\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Empty values aren't corrupted for PSUs\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - Initializing Values, previous data ain't corrupted for PSUs\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 3 - Initializing Values, empty values are correctly filled for PSUs\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 4 - Setting default Values and saving values for SnIs\n");
	iprintf(" ~results[5]: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - Restore Saved Values for SnIs\n");
	iprintf(" ~results[6]: %s\n", (results[6]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 5 - Empty values aren't corrupted for SnIs\n");
	iprintf(" ~results[7]: %s\n", (results[7]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 6 - Initializing Values, previous data ain't corrupted for SnIs\n");
	iprintf(" ~results[8]: %s\n", (results[8]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 6 - Initializing Values, empty values are correctly filled for SnIs\n");
	iprintf(" ~results[9]: %s\n", (results[9]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]&&results[6]&&results[7]&&results[8]&&results[9]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));
	return resultTotal;
}



BOOL TEST_MonitorCTRL( void ){
	iprintf("\n\n\n\n===============TEST_MonitorCTRL===============\n");
	// Initialize all values
	initializeValuesPSUsSnIs();

	// Set Monitoring at the first point and taking control of Monitor Function
	sequentialMode_MonitorCTRL_Task(ON);
	testMode_MonitorCTRL_Measure (ON);
	iddleMode_AlarmCTRL_Task(ON);
	OSTimeDly(2);	// Wait for MonitorTask to stop at testMode Semaphore

	// Test with high values
	TEST_MonitorCTRL_HIGHVALUES();
	WAIT_FOR_KEYBOARD

	// Test with nominal values
	TEST_MonitorCTRL_NOMINALVALUES();
	WAIT_FOR_KEYBOARD

	//Results
	iprintf("\n TEST_MonitorCTRL\n");
	iprintf("\n PART 1 - Stored high values for PSU voltage\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 1 - Stored high values for PSU current\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 1 - Stored high values for SnI voltage\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Stored nominal values for PSU voltage\n");
	iprintf(" ~results[3]: %s\n", (results[3]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Stored nominal values for PSU current\n");
	iprintf(" ~results[4]: %s\n", (results[4]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Stored nominal values for SnI voltage\n");
	iprintf(" ~results[5]: %s\n", (results[5]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]&&results[3]&&results[4]&&results[5]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));

	iddleMode_AlarmCTRL_Task(OFF);
	testMode_MonitorCTRL_Measure(OFF);
	sequentialMode_MonitorCTRL_Task(OFF);
	return resultTotal;
}

BOOL TEST_SwitchOnCTRL( void ){
	iprintf("\n\n\n\n===============TEST_SwitchOnCTRL===============\n");

	sequentialMode_MonitorCTRL_Task(ON);
	iddleMode_AlarmCTRL_Task(ON);
	testMode_SwitchOnCTRL_Task(ON);
	int progTicks[PSU_NUMBER];

	iprintf("\n\n\n\n1. Initialize values for all (initializationTimer = psuNum)\n[ ");
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		psuList[test_Num].initializationTimer = test_Num;
		progTicks[test_Num]= psuList[test_Num].initializationTimer;
		iprintf("%d, ", progTicks[test_Num]);
	}
	iprintf("]\nBegin initialization\n");
	WAIT_FOR_KEYBOARD
	switchONPSUs();
	// checking for data correction
	iprintf("Checking for data correction:\n");
	WAIT_FOR_KEYBOARD
	results[0]=true;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[0]&= (testMode_psu_initializationTimer[test_Num]==progTicks[test_Num]);
	}
	iprintf("realTicks [");	printBuffer(testMode_psu_initializationTimer, PSU_NUMBER);	iprintf("\n");
	iprintf("progTicks [");	printBuffer(progTicks, PSU_NUMBER);	iprintf("\n");
	iprintf("Same Values? (y/n): ");WAIT_FOR_KEYBOARD
	iprintf("\n");
	results[1] = (testChar=='y' || testChar =='Y');

	iprintf("\n\n\n\n2. Checking all the array\n");
	WAIT_FOR_KEYBOARD
	results[2] = true;
	iprintf("comparison [");
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		iprintf ("%d==%d,  ", testMode_psu_initializationTimer[test_Num], (int)test_Num);
		results[2]&= (testMode_psu_initializationTimer[test_Num]==(int)test_Num);
	}
	iprintf("]\n");
	//Results
	iprintf("\n TEST_SwitchOnCTRL\n");
	iprintf("\n PART 1 - Compared values of initializationTimer with RAM ones\n");
	iprintf(" ~results[0]: %s\n", (results[0]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 1 - Initialized & printed values of initializationTimer\n");
	iprintf(" ~results[1]: %s\n", (results[1]?"PASSED":"NOT PASSED"));
	iprintf("\n PART 2 - Compared values of initializationTimer with programmed ones\n");
	iprintf(" ~results[2]: %s\n", (results[2]?"PASSED":"NOT PASSED"));
	resultTotal = (results[0]&&results[1]&&results[2]);
	iprintf("\n OVERALL RESULT: %s\n", (resultTotal?"PASSED":"NOT PASSED"));

	testMode_SwitchOnCTRL_Task(OFF);
	iddleMode_AlarmCTRL_Task(OFF);
	sequentialMode_MonitorCTRL_Task(OFF);
	return resultTotal;
}


BOOL TEST_VoltCurrCTRL( void ){
	int adcCounts; float value = 0;
	int samplingFunction=FUNCTION_PSU_VOLTAGE; int num = 0;
	sequentialMode_MonitorCTRL_Task(ON);
	iddleMode_AlarmCTRL_Task(ON);
	iprintf("\n\n\n\n===============TEST_VoltCurrCTRL===============\n");
	iprintf("\n\n\n\n1. Minimum AGC gain\n");

	minAGC();
	adcCounts = 0x800;
	iprintf("\nadcCounts: 0x800\n");
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, num);
	iprintf("adcCounts to Volt: %s\n", ftos(value));
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, num);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	WAIT_FOR_KEYBOARD
	value = 21;
	iprintf("\nValue: 21\n");
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, num);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, num);
	printf("adcCounts to Volt: %f\n", value);
	WAIT_FOR_KEYBOARD


	samplingFunction = FUNCTION_PSU_CURRENT;
	value = 0.7;
	iprintf("\nValue: 0.7 in 1\n");
	setGainAGC(66.96);
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, 1);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, 1);
	printf("adcCounts to Volt: %f\n", value);

	value = -0.7;
	iprintf("\nValue: -0.7 in 10\n");
	minAGC();
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, 10);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, 10);
	iprintf("adcCounts to Volt: %s\n", ftos(value));
	iprintf("ScaleChanged\n");
	scaleGainAGC(MIDSCALE_ADC/adcCounts);
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, 10);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, 10);
	printf("adcCounts to Volt: %f\n", value);
	minAGC();
	iprintf( "Value = %s\n", ftos(ADCCountsToVoltORCurr(0x2c, FUNCTION_PSU_CURRENT, 10)));
	scaleGainAGC(MIDSCALE_ADC/adcCounts);
	iprintf( "Value = %s\n", ftos(ADCCountsToVoltORCurr(0xa91, FUNCTION_PSU_CURRENT, 10)));


	WAIT_FOR_KEYBOARD



	value = -21;
	iprintf("\nValue: -21\n");
	adcCounts = VoltORCurrToADCCounts (value, samplingFunction, 10);
	iprintf("adcCounts returned: 0x%x\n", adcCounts);
	value =ADCCountsToVoltORCurr (adcCounts, samplingFunction, 10);
	printf("adcCounts to Volt: %f\n", value);
	WAIT_FOR_KEYBOARD
	sequentialMode_MonitorCTRL_Task(OFF);
	iddleMode_AlarmCTRL_Task(OFF);
	return true;
}
//-----------------------------------------------------------------------------------------------------//
//-------------------------------------   PARTIAL TESTS    --------------------------------------------//
//-----------------------------------------------------------------------------------------------------//



void TEST_AlarmCTRL_PSU1( void ){
	iprintf("\n\n\n\n1. Inferior Voltage Alarm Triggering with no time. PSU 1 should be triggered, 4 shouldn't\n");
	defaultValuesPSU(1);
	defaultValuesPSU(4);
	setalarmLimitTimesPSU(0, INFERIOR, VOLTAGE, 1);
	setalarmLimitTimesPSU(0, INFERIOR, VOLTAGE, 4);
	setalarmWatchPSU(ON, INFERIOR, VOLTAGE, 1);
	setalarmWatchPSU(ON, INFERIOR, VOLTAGE, 4);
	// Simulating a value lower than 13 (default inferior limit) for voltage
	psuList[1].vOut = 12;
	psuList[4].vOut = 14;
	alarmCheck(1,INFERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,INFERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" First comparison (v1 = 12, v4 =14, vLLim=13\n");
	iprintf(" alarmStatus (1) = %s (should be TRUE)(AC = %d)\n", (getPSU(1).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(INFERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getPSU(4).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(INFERIOR,VOLTAGE)]);
	results[0] = getPSU(1).alarmStatus[_(INFERIOR,VOLTAGE)] == true;
	results[1] = getPSU(4).alarmStatus[_(INFERIOR,VOLTAGE)] == false;
	// Simulating a value higher than 13 for voltage
	psuList[1].vOut = 14;
	alarmCheck(1,INFERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getPSU(1).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(INFERIOR,VOLTAGE)]);
	results[0] &= getPSU(1).alarmStatus[_(INFERIOR,VOLTAGE)] == false;
	results[1] &= getPSU(4).alarmStatus[_(INFERIOR,VOLTAGE)] == false;
	setalarmWatchPSU(OFF, INFERIOR, VOLTAGE, 1);
	setalarmWatchPSU(OFF, INFERIOR, VOLTAGE, 4);
}


void TEST_AlarmCTRL_PSU2( void ){
	iprintf("\n\n\n\n2. Superior Voltage Alarm Triggering with 3 times. PSU 1 should be triggered, 4 shouldn't\n");
	resetAlarms();
	setalarmLimitTimesPSU(3, SUPERIOR, VOLTAGE, 1);
	setalarmLimitTimesPSU(3, SUPERIOR, VOLTAGE, 4);
	setalarmWatchPSU(ON, SUPERIOR, VOLTAGE, 1);
	setalarmWatchPSU(ON, SUPERIOR, VOLTAGE, 4);
	// Simulating a value higher than 17 (default superior limit) for voltage
	psuList[1].vOut = 18;
	psuList[4].vOut = 15;
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" First comparison (v1 = 18, v4 =15, vULim=17\n");
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[2] = getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	results[3] = getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	results[2] &= getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	results[3] &= getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" Third comparison (v1 = 18, v4 =15, vULim=17\n");
	iprintf(" alarmStatus (1) = %s (should be TRUE)(AC = %d)\n", (getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[2] &= getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	results[3] &= getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	// Simulating a value lower than 18 for voltage
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	results[2] &= getPSU(1).alarmCounters[_(SUPERIOR,VOLTAGE)] == getPSU(1).alarmLimitTimes[_(SUPERIOR,VOLTAGE)];
	psuList[1].vOut = 16;
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" Fourth comparison (v1 = 16, v4 =15, vULim=17\n");
	iprintf(" alarmStatus (1) = %s (should be TRUE)(AC = %d)\n", (getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[2] &= getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	results[3] &= getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(1,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,PSU_TYPE_LIST);
	iprintf(" Sixth comparison (v1 = 16, v4 =15, vULim=17\n");
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[2] &= getPSU(1).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	results[3] &= getPSU(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	setalarmWatchPSU(OFF, SUPERIOR, VOLTAGE, 1);
	setalarmWatchPSU(OFF, SUPERIOR, VOLTAGE, 4);
}


void TEST_AlarmCTRL_PSU3( void ){
	iprintf("\n\n\n\n3. Inferior Current Alarm Triggering with 2 times and Disconnection. PSU 1 should be triggered, 4 shouldn't\n");
	resetAlarms();
	setalarmLimitTimesPSU(2, INFERIOR, CURRENT, 1);
	setalarmLimitTimesPSU(2, INFERIOR, CURRENT, 4);
	setalarmWatchPSU(ON, INFERIOR, CURRENT, 1);
	setalarmWatchPSU(ON, INFERIOR, CURRENT, 4);
	// Simulating a value lower than 0.3 (default superior limit) for voltage
	psuList[1].cOut = 0.15;
	psuList[4].cOut = 0.7;
	alarmCheck(1,INFERIOR, CURRENT,PSU_TYPE_LIST);
	alarmCheck(4,INFERIOR, CURRENT,PSU_TYPE_LIST);
	iprintf(" First comparison (c1 = 0.15, c4 =0.7, cLLim=0.3\n");
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getPSU(1).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(INFERIOR, CURRENT)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getPSU(4).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(INFERIOR, CURRENT)]);
	results[4] = getPSU(1).alarmStatus[_(INFERIOR, CURRENT)] == false;
	results[5] = getPSU(4).alarmStatus[_(INFERIOR, CURRENT)] == false;
	alarmCheck(1,INFERIOR, CURRENT,PSU_TYPE_LIST);
	alarmCheck(4,INFERIOR, CURRENT,PSU_TYPE_LIST);
	iprintf(" Second comparison (c1 = 0.15, c4 =0.7, cLLim=0.3\n");
	iprintf(" alarmStatus (1) = %s (should be TRUE)(AC = %d)\n", (getPSU(1).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(INFERIOR, CURRENT)]);
	iprintf(" alarmStatus (4) = %s (should be %s)(AC = %d)\n", (getPSU(4).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), (0.7<=0.3?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(INFERIOR, CURRENT)]);
	results[4] &= getPSU(1).alarmStatus[_(INFERIOR, CURRENT)] == true;
	results[5] &= getPSU(4).alarmStatus[_(INFERIOR, CURRENT)] == false;
	alarmCheck(1,INFERIOR, CURRENT,PSU_TYPE_LIST);
	results[4] = getPSU(1).alarmCounters[_(INFERIOR, CURRENT)] == getPSU(1).alarmLimitTimes[_(INFERIOR, CURRENT)];
	setalarmWatchPSU(OFF, INFERIOR, CURRENT, 1);
	setalarmWatchPSU(OFF, INFERIOR, CURRENT, 4);
	alarmCheck(1,INFERIOR, CURRENT,PSU_TYPE_LIST);
	alarmCheck(4,INFERIOR, CURRENT,PSU_TYPE_LIST);
	iprintf(" Third comparison (c1 = 0.15, c4 =0.7, cLLim=0.3) - Disconnecting\n");
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getPSU(1).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), getPSU(1).alarmCounters[_(INFERIOR, CURRENT)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getPSU(4).alarmStatus[_(INFERIOR, CURRENT)]?"TRUE":"FALSE"), getPSU(4).alarmCounters[_(INFERIOR, CURRENT)]);
	results[4] &= getPSU(1).alarmStatus[_(INFERIOR, CURRENT)] == false;
	results[5] &= getPSU(4).alarmStatus[_(INFERIOR, CURRENT)] == false;
}


void TEST_AlarmCTRL_PSU4( void ){
	iprintf("\n\n\n\n4. Multiple Alarm Triggering for Upper Current with 2 times and Disconnection. All PSUs triggered. Using updateAlarms()\n");
	int i;
	resetAlarms();
	for (i=0;i<PSU_NUMBER;i++){
		psuList[i].psuStatus=ON;
		psuList[i].cOut = i+3.5; // >3 (Default Upper Current Limit)
		psuList[i].vOut = i+17.5;// >17 (Default Upper Voltage Limit)
		psuList[i].alarmWatch[_(SUPERIOR, CURRENT)]=ON;	 // Should be triggered
		psuList[i].alarmWatch[_(INFERIOR, CURRENT)]=OFF;
		psuList[i].alarmWatch[_(INFERIOR, VOLTAGE)]=OFF;
		psuList[i].alarmWatch[_(SUPERIOR, VOLTAGE)]=OFF; //Shouldn't be triggered
		psuList[i].alarmLimitTimes[_(SUPERIOR, CURRENT)]=2;	 // Should be triggered
		psuList[i].alarmLimitTimes[_(INFERIOR, CURRENT)]=2;
		psuList[i].alarmLimitTimes[_(INFERIOR, VOLTAGE)]=2;
		psuList[i].alarmLimitTimes[_(SUPERIOR, VOLTAGE)]=2; //Shouldn't be triggered
	}
	updateAlarms();
	results[6] = true;
	iprintf ("First time (all to 0): {");
	for (i=0;i<PSU_NUMBER;i++){
		results[6] &= (psuList[i].alarmStatus[_(SUPERIOR, CURRENT)] == false) && (psuList[i].alarmCounters[_(SUPERIOR, CURRENT)]==1);
		results[6] &= (psuList[i].alarmStatus[_(SUPERIOR, VOLTAGE)] == false) && (psuList[i].alarmCounters[_(SUPERIOR, VOLTAGE)]==0);
		iprintf (" %d,", psuList[i].alarmStatus[_(SUPERIOR, CURRENT)]);
	}
	iprintf ("}\n");
	iprintf ("Second time (all to 1): {");
	updateAlarms();
	for (i=0;i<PSU_NUMBER;i++){
		results[6] &= (psuList[i].alarmStatus[_(SUPERIOR, CURRENT)] == true) && (psuList[i].alarmCounters[_(SUPERIOR, CURRENT)]==2);
		results[6] &= (psuList[i].alarmStatus[_(SUPERIOR, VOLTAGE)] == false) && (psuList[i].alarmCounters[_(SUPERIOR, VOLTAGE)]==0);
		iprintf (" %d,", psuList[i].alarmStatus[_(SUPERIOR, CURRENT)]);
	}
	iprintf ("}\n");
	resetAlarms();
}


void TEST_AlarmCTRL_SnI1( void ){
	iprintf("\n\n\n\n5. Inferior Voltage Alarm Triggering with no time. SnI 13 should be triggered, 4 shouldn't\n");
	defaultValuesSnI(13);
	defaultValuesSnI(4);
	setalarmLimitTimesSnI(0, INFERIOR, VOLTAGE, 13);
	setalarmLimitTimesSnI(0, INFERIOR, VOLTAGE, 4);
	setalarmWatchSnI(ON, INFERIOR, VOLTAGE, 13);
	setalarmWatchSnI(ON, INFERIOR, VOLTAGE, 4);
	// Simulating a value lower than -15 (default inferior limit for sni 13) for voltage
	sniList[13].vOut = -15;
	sniList[4].vOut = 16;
	alarmCheck(13,INFERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,INFERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" First comparison (v13 = -15 (-14), v4 =16(16))\n");
	iprintf(" alarmStatus (13) = %s (should be TRUE)(AC = %d)\n", (getSnI(13).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(INFERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getSnI(4).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(4).alarmCounters[_(INFERIOR,VOLTAGE)]);
	results[7] = (getSnI(13).alarmStatus[_(INFERIOR,VOLTAGE)] == true) && (getSnI(4).alarmStatus[_(INFERIOR,VOLTAGE)] == false);
	// Simulating a value higher than -14 for voltage
	sniList[13].vOut = -12;
	alarmCheck(13,INFERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" alarmStatus (13) = %s (should be FALSE)(AC = %d)\n", (getSnI(13).alarmStatus[_(INFERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(INFERIOR,VOLTAGE)]);
	results[7] &= getSnI(13).alarmStatus[_(INFERIOR,VOLTAGE)] == false;
	results[7] &= getSnI(4).alarmStatus[_(INFERIOR,VOLTAGE)] == false;
	setalarmWatchSnI(OFF, INFERIOR, VOLTAGE, 13);
	setalarmWatchSnI(OFF, INFERIOR, VOLTAGE, 4);
}


void TEST_AlarmCTRL_SnI2( void ){
	iprintf("\n\n\n\n6. Superior Voltage Alarm Triggering with 3/2 times. Both should be triggered\n");
	resetAlarms();
	setalarmLimitTimesSnI(3, SUPERIOR, VOLTAGE, 13);
	setalarmLimitTimesSnI(2, SUPERIOR, VOLTAGE, 4);
	setalarmWatchSnI(ON, SUPERIOR, VOLTAGE, 13);
	setalarmWatchSnI(ON, SUPERIOR, VOLTAGE, 4);
	// Simulating a value higher than -10 (default superior limit) for voltage
	sniList[13].vOut = -5;
	sniList[4].vOut = 25;
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" First comparison (v13 = -5(-10), v4 =25(18))\n");
	iprintf(" alarmStatus (1) = %s (should be FALSE)(AC = %d)\n", (getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[8] = (getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)] == false) && (getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false);
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	results[8] &= getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	results[8] &= getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" Third comparison (v13 = -5(-10), v4 =25(18))\n");
	iprintf(" alarmStatus (13) = %s (should be TRUE)(AC = %d)\n", (getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be TRUE)(AC = %d)\n", (getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[8] &= getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	results[8] &= getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	// Simulating a value lower than 18 for voltage
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	results[8] &= getSnI(13).alarmCounters[_(SUPERIOR,VOLTAGE)] == getSnI(13).alarmLimitTimes[_(SUPERIOR,VOLTAGE)];
	sniList[13].vOut = -12;
	setalarmWatchSnI(OFF, SUPERIOR, VOLTAGE, 4);
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" Fourth comparison (v13 = -12(-10), v4 =25(18))\n");
	iprintf(" alarmStatus (13) = %s (should be TRUE)(AC = %d)\n", (getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d) -Disconnected\n", (getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[8] &= getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)] == true;
	results[8] &= getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(13,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	alarmCheck(4,SUPERIOR,VOLTAGE,SnI_TYPE_LIST);
	iprintf(" Sixth comparison (v13 = -12(-10), v4 =25(18))\n");
	iprintf(" alarmStatus (13) = %s (should be FALSE)(AC = %d)\n", (getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(13).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	iprintf(" alarmStatus (4) = %s (should be FALSE)(AC = %d)\n", (getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)]?"TRUE":"FALSE"), getSnI(4).alarmCounters[_(SUPERIOR,VOLTAGE)]);
	results[8] &= getSnI(13).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	results[8] &= getSnI(4).alarmStatus[_(SUPERIOR,VOLTAGE)] == false;
	setalarmWatchSnI(OFF, SUPERIOR, VOLTAGE, 13);
	setalarmWatchSnI(OFF, SUPERIOR, VOLTAGE, 4);
}

void TEST_AlarmCTRL_SnI3( void ){
	iprintf("\n\n\n\n7. Multiple Alarm Triggering for Upper Voltage with 2 times and Disconnection. All SnIs triggered. Using updateAlarms()\n");
	int j;
	resetAlarms();
	config_AlarmCTRLUpdate_SnI(true);
	for (j=0;j<SnI_NUMBER;j++){
		sniList[j].vOut = sniList[j].alarmLimitValues[_(SUPERIOR, VOLTAGE)] + 2; //all superior limits exceeded
		sniList[j].alarmWatch[_(INFERIOR, VOLTAGE)]=ON;
		sniList[j].alarmWatch[_(SUPERIOR, VOLTAGE)]=ON; //Should be triggered
		sniList[j].alarmLimitTimes[_(INFERIOR, VOLTAGE)]=2;
		sniList[j].alarmLimitTimes[_(SUPERIOR, VOLTAGE)]=2; //Should be triggered
	}
	updateAlarms();
	results[9] = true;
	iprintf ("First time (all to 0): {");
	for (j=0;j<SnI_NUMBER;j++){
		results[9] &= (sniList[j].alarmStatus[_(INFERIOR, VOLTAGE)] == false) && (sniList[j].alarmCounters[_(INFERIOR, VOLTAGE)]==0);
		results[9] &= (sniList[j].alarmStatus[_(SUPERIOR, VOLTAGE)] == false) && (sniList[j].alarmCounters[_(SUPERIOR, VOLTAGE)]==1);
		iprintf (" %d,", sniList[j].alarmStatus[_(SUPERIOR, VOLTAGE)]);
	}
	iprintf ("}\n");
	iprintf ("Second time (all to 1): {");
	updateAlarms();
	for (j=0;j<PSU_NUMBER;j++){
		results[9] &= (sniList[j].alarmStatus[_(INFERIOR, VOLTAGE)] == false) && (sniList[j].alarmCounters[_(INFERIOR, VOLTAGE)]==0);
		results[9] &= (sniList[j].alarmStatus[_(SUPERIOR, VOLTAGE)] == true) && (sniList[j].alarmCounters[_(SUPERIOR, VOLTAGE)]==2);
		iprintf (" %d,", sniList[j].alarmStatus[_(SUPERIOR, VOLTAGE)]);
	}
	iprintf ("}\n");
	resetAlarms();
}



void TEST_FlashMemCTRL_PSU (void){
	iprintf("\n\n\n\n1. Setting default Values for PSUs 1 and 4, and saving values\n");
	defaultValuesPSU(1);
	defaultValuesPSU(4);
//	printValuesPSU(1);
//	printValuesPSU(4);
	psuList[5].alarmProtocolShutdown[0] = 0;
	saveInFlashValuesPSUsSNIs();
	results[0] = (getPSU(4).alarmProtocolShutdown[0] == demux4to16(4+1)) && (getPSU(1).alarmProtocolShutdown[0] == demux4to16(1+1));
	iprintf(" Saved alarmProtocolShutdown (1) = %d (should be %d)\n", getPSU(1).alarmProtocolShutdown[0], demux4to16(1+1));
	iprintf(" Saved alarmProtocolShutdown (4) = %d (should be %d)\n", getPSU(4).alarmProtocolShutdown[0], demux4to16(4+1));
	printf("\n\n\n\n2. Loading Values and checking for data validity\n");
	psuList[4].alarmProtocolShutdown[0] = 0;
	psuList[5].alarmProtocolShutdown[0] = 2;
	loadFlashValuesPSUs();
//	printValuesPSU(5);
//	printValuesPSU(4);
	results[1] = getPSU(4).alarmProtocolShutdown[0] == demux4to16(4+1);
	results[2] = getPSU(5).alarmProtocolShutdown[0] == 0;
	iprintf(" Loaded alarmProtocolShutdown (4) = %d (should be %d)\n", getPSU(4).alarmProtocolShutdown[0], demux4to16(4+1));
	iprintf(" Loaded alarmProtocolShutdown (5) = %d (should be %d)\n", getPSU(5).alarmProtocolShutdown[0], 0);
	iprintf("\n\n\n\n3. Initializing Values, printing PSUs 4 and 5, and checking for data validity\n");
	psuList[4].alarmProtocolShutdown[0] = 0;
	psuList[5].alarmProtocolShutdown[0] = 0;
	initializeValuesPSUsSnIs();
//	printValuesPSU(4);
//	printValuesPSU(5);
	results[3] = getPSU(4).alarmProtocolShutdown[0] == demux4to16(4+1);
	results[4] = getPSU(5).alarmProtocolShutdown[0] == demux4to16(5+1);
	iprintf(" Initialized alarmProtocolShutdown (4) = %d (should be %d)\n", getPSU(4).alarmProtocolShutdown[0], demux4to16(4+1));
	iprintf(" Initialized alarmProtocolShutdown (5) = %d (should be %d)\n", getPSU(5).alarmProtocolShutdown[0], demux4to16(5+1));
}


void TEST_FlashMemCTRL_SnI(void){
	iprintf("\n\n\n\n4. Setting default Values for SnIs 4 and 13, and saving values\n");
	defaultValuesSnI(13);
	defaultValuesSnI(4);
	sniList[5].nominalVolt = 0;
	saveInFlashValuesPSUsSNIs();
//	printValuesSnI(4);
//	printValuesSnI(5);
//	printValuesSnI(13);
	iprintf(" Saved InfVoltValue(4) = %s ", ftos(getSnI(4).alarmLimitValues[0]));iprintf("(should be < %s)\n", ftos(getSnI(4).nominalVolt));
	iprintf(" Saved SupVoltValue(4) = %s ", ftos(getSnI(4).alarmLimitValues[1]));iprintf("(should be > %s)\n", ftos(getSnI(4).nominalVolt));
	iprintf(" Saved InfVoltValue(13) = %s ", ftos(getSnI(13).alarmLimitValues[0]));iprintf("(should be < %s)\n", ftos(getSnI(13).nominalVolt));
	iprintf(" Saved SupVoltValue(13) = %s ", ftos(getSnI(13).alarmLimitValues[1]));iprintf("(should be > %s)\n", ftos(getSnI(13).nominalVolt));
	iprintf(" Saved nominalVoltage(5) = %s (should be 0)\n", ftos(getSnI(5).nominalVolt));
	//Check Values
	results[5] = ((getSnI(4).alarmLimitValues[0] < getSnI(4).nominalVolt) && (getSnI(4).alarmLimitValues[1] > getSnI(4).nominalVolt)) && ((getSnI(13).alarmLimitValues[0] < getSnI(13).nominalVolt) && (getSnI(13).alarmLimitValues[1] > getSnI(13).nominalVolt));
	//end Check Values
	iprintf("\n\n\n\n5. Loading Values and checking for data validity\n");
	getSnI(4).nominalVolt = 0;
	getSnI(5).nominalVolt = 2;
	loadFlashValuesSNIs();
//	printValuesSnI(4);
//	printValuesSnI(5);
	iprintf(" Loaded nominalVoltage(4) = %s (should be > 0)\n", ftos(getSnI(4).nominalVolt));
	iprintf(" Loaded nominalVoltage(13) = %s (should be < 0)\n", ftos(getSnI(13).nominalVolt));
	iprintf(" Loaded nominalVoltage(5) = %s (should be 0)\n", ftos(getSnI(5).nominalVolt));
	//Check Values
	results[6] = (getSnI(4).nominalVolt != 0) && (getSnI(13).nominalVolt != 0);
	results[7] = getSnI(5).nominalVolt == 0;
	//end Check Values
	printf("\n\n\n\n6. Initializing Values, printing SnIs 4 and 5, and checking for data validity\n");
	getSnI(4).nominalVolt = 0;
	getSnI(5).nominalVolt = 0;
	initializeValuesPSUsSnIs();
//	printValuesSnI(4);
//	printValuesSnI(5);
	iprintf(" Initialized nominalVoltage(4) = %s (should be > 0)\n", ftos(getSnI(4).nominalVolt));
	iprintf(" Initialized nominalVoltage(13) = %s (should be < 0)\n", ftos(getSnI(13).nominalVolt));
	iprintf(" Initialized nominalVoltage(5) = %s (should be > 0)\n", ftos(getSnI(5).nominalVolt));
	//Check Values
	results[8] = (getSnI(4).nominalVolt > 0) && (getSnI(13).nominalVolt < 0);
	results[9] = (getSnI(5).nominalVolt > 0 );
	//end Check Values
}



void TEST_MonitorCTRL_HIGHVALUES( void ){
	int auxMonNum; int auxSampFunc;

	// Set muxes and values for first reading
	monitorSamplFunc = FUNCTION_PSU_VOLTAGE;monitorNum = 0;
	setMUX( monitorSamplFunc, (BYTE)monitorNum );
	minAGC();

	iprintf("\n\n\n\n1. Storing high Values of Voltage, then Current(0x800+10*psuSniNum at minAGC) for all PSUs and SnIs\n");
	for (test_Num = 0; test_Num<(PSU_NUMBER*2+SnI_NUMBER); test_Num++){
		testMode_set_Measure ( test_measure );
		auxMonNum = monitorNum; auxSampFunc=monitorSamplFunc;
		sequentialMode_MonitorCTRL_Task(ON);					// used to advance one in the sampling loop
		iprintf("Meas: 0x%x   -   %s[%d].%s=0x%x    -   N:%d\n", test_measure, (auxSampFunc==FUNCTION_SnI_VOLTAGE?"sni":"psu"), auxMonNum, (auxSampFunc==FUNCTION_PSU_CURRENT?"cOut":"vOut"), VoltORCurrToADCCounts((auxSampFunc==FUNCTION_SnI_VOLTAGE?sniList[auxMonNum].vOut:(auxSampFunc==FUNCTION_PSU_CURRENT?psuList[auxMonNum].cOut:psuList[auxMonNum].vOut)), auxSampFunc, auxMonNum), test_Num);
		test_measure+=0x10;
	}
	iprintf("Voltage PSU:\n");
	test_measure = 0x800;
	results[0] = true;
	auxSampFunc = FUNCTION_PSU_VOLTAGE; auxMonNum=0;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[0] = VoltORCurrToADCCounts(psuList[test_Num].vOut, auxSampFunc, auxMonNum) == test_measure;
		iprintf("[%d]=%sV   ", test_Num, ftos(psuList[test_Num].vOut));
		test_measure+=0x10;
		auxMonNum++;
	}
	iprintf("\n");
	iprintf("Current PSU:\n");
	results[1] = true;
	auxSampFunc = FUNCTION_PSU_CURRENT; auxMonNum=0;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[1] = VoltORCurrToADCCounts(psuList[test_Num].cOut, auxSampFunc, auxMonNum) == test_measure;
		iprintf("[%d]=%sA   ", test_Num, ftos(psuList[test_Num].cOut));
		test_measure+=0x10;
		auxMonNum++;
	}
	iprintf("\n");
	iprintf("Voltage SnI:\n");
	results[2] = true;
	auxSampFunc = FUNCTION_SnI_VOLTAGE; auxMonNum=0;
	for (test_Num = 0; test_Num<SnI_NUMBER; test_Num++){
		results[2] = VoltORCurrToADCCounts(sniList[test_Num].vOut, auxSampFunc, auxMonNum) == test_measure;
		iprintf("[%d]=%sV   ", test_Num, ftos(sniList[test_Num].vOut));
		test_measure+=0x10;
		auxMonNum++;
	}
}


void TEST_MonitorCTRL_NOMINALVALUES( void ){
	int auxMonNum; WORD measure;
	resetMeasuresAll ();

	// Set muxes and values for first reading
	monitorSamplFunc = FUNCTION_PSU_VOLTAGE;monitorNum = 0;
	setMUX( monitorSamplFunc, (BYTE)monitorNum );

	iprintf("\n\n\n\n2. Storing nominal Values of Voltage and Current for all PSUs and SnIs\n");
	iprintf("   and setting all the alarms ON\n");

	// PSU Voltage
	iprintf("PSU Voltage\n");
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		measure = VoltORCurrToADCCounts ((test_Num<=SF5_B?22:-10), FUNCTION_PSU_VOLTAGE, test_Num);
		testMode_set_Measure ( measure ); // NOMINAL VALUE
		auxMonNum = monitorNum;
		sequentialMode_MonitorCTRL_Task(ON);					// used to advance one in the sampling loop
		while (auxMonNum == monitorNum){ // If the test_measure wasn't valid, extra loop cycles are required
			measure = VoltORCurrToADCCounts ((test_Num<=SF5_B?22:-10), FUNCTION_PSU_VOLTAGE, test_Num); // Refresh for new AGC scale
			testMode_set_Measure ( measure ); // NOMINAL VALUE
			sequentialMode_MonitorCTRL_Task(ON);
		}
		iprintf("Meas: 0x%x (+22/-16)  -   psu[%d].vOut=%s\n", measure, auxMonNum,ftos(psuList[auxMonNum].vOut));

	}
	// PSU Current
	iprintf("PSU Current\n");
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		measure = VoltORCurrToADCCounts (0.7, FUNCTION_PSU_CURRENT, test_Num);
		testMode_set_Measure ( measure ); // NOMINAL VALUE
		auxMonNum = monitorNum;
		sequentialMode_MonitorCTRL_Task(ON);					// used to advance one in the sampling loop
		while (auxMonNum == monitorNum){ // If the test_measure wasn't valid, extra loop cycles are required
			measure = VoltORCurrToADCCounts (0.7, FUNCTION_PSU_CURRENT, test_Num);
			testMode_set_Measure ( measure ); // NOMINAL VALUE
			sequentialMode_MonitorCTRL_Task(ON);
		}
		iprintf("Meas: 0x%x (+-0.7)  -   psu[%d].cOut=%s\n", measure, auxMonNum, ftos(psuList[auxMonNum].cOut));
	}
	// SnI Voltage
	iprintf("SNI Voltage\n");
	for (test_Num = 0; test_Num<SnI_NUMBER; test_Num++){
		measure = VoltORCurrToADCCounts (sniList[test_Num].nominalVolt, FUNCTION_SnI_VOLTAGE, test_Num);
		testMode_set_Measure ( measure ); // NOMINAL VALUE
		auxMonNum = monitorNum;
		sequentialMode_MonitorCTRL_Task(ON);					// used to advance one in the sampling loop
		while (auxMonNum == monitorNum){ // If the test_measure wasn't valid, extra loop cycles are required
			measure = VoltORCurrToADCCounts (sniList[test_Num].nominalVolt, FUNCTION_SnI_VOLTAGE, test_Num);
			testMode_set_Measure ( measure ); // NOMINAL VALUE
			sequentialMode_MonitorCTRL_Task(ON);
		}
		iprintf("Meas: 0x%x(%sV)  -   sni[%d].vOut=%s\n", measure, ftos(sniList[test_Num].nominalVolt), auxMonNum, ftos(sniList[auxMonNum].vOut, 2, 1, 1));
	}
	iprintf ("Updating all alarms (only PSU superior voltages should be triggered)\n");
	resetAlarms();
	toggleAlarms(ON);
	for (test_Num=0; test_Num<PSU_NUMBER; test_Num++){
		psuList[test_Num].psuStatus = ON;
	}
	updateAlarms();
	results[3]=true;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[3] &= psuList[test_Num].alarmLimitReached[_(INFERIOR,VOLTAGE)] == 0;
		results[3] &= psuList[test_Num].alarmLimitReached[_(SUPERIOR,VOLTAGE)] == 1;
	}
	results[4] = true;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[4] &= psuList[test_Num].alarmLimitReached[_(INFERIOR,CURRENT)] == 0;
		results[4] &= psuList[test_Num].alarmLimitReached[_(SUPERIOR,CURRENT)] == 0;
	}
	results[5] = true;
	for (test_Num = 0; test_Num<PSU_NUMBER; test_Num++){
		results[5] &= sniList[test_Num].alarmLimitReached[_(INFERIOR,VOLTAGE)] == 0;
		results[5] &= sniList[test_Num].alarmLimitReached[_(SUPERIOR,VOLTAGE)] == 0;
	}
	iprintf("\n");
}
