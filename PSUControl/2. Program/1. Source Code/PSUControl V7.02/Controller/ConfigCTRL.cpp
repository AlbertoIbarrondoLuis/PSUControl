/*
 * ConfigCTRL.cpp
 *
 *	Definition of several configuration values (alarm update period, SnI included in alarmTask and monitorTask),
 *	methods to pause alarmTask and monitorTask (setting them into idleMode and sequentialMode respectively),
 *	and to set testMode for monitorTask and SwitchONPSUsTask.
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Controller/Controller.h"


//==============================================VARIABLES==============================================//

// Config
BOOL config_MonitorCTRL_SnI_FLAG = CONFIG_INIT_MONITORCTRL_SnI_FLAG;			// Supply & Internal voltages																		// included in a2d loop or not
BOOL config_AlarmCTRLUpdate_SnI_FLAG = CONFIG_INIT_ALARMCTRL_UPDATE_SnI_FLAG;	// Includes/excludes SnI voltage
																				// alarms when using updateAlarms(void)
int alarmUpdatePeriodx50MS = CONFIG_INIT_ALARMCTRL_UPDATE_PERIOD_x50MS;			// Period x50ms for alarm update task
BOOL consoleOutputRDAC = CONFIG_INIT_CONSOLE_OUTPUT_RDAC;						// Toggles console messages for RDAC functions
BOOL consoleOutputI2C = CONFIG_INIT_CONSOLE_OUTPUT_I2CnSPI;						// Toggles console messages for I2C and SPI functions


// Pause Tasks
BOOL idleMode_AlarmCTRL_Task_FLAG=CONFIG_INIT_PAUSE_TASKS;	// Sets Alarm Task into idle mode
BOOL seqMode_MonitorCTRL_Task_FLAG=CONFIG_INIT_PAUSE_TASKS;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem

// Testing Booleans
BOOL testMode_MonitorCTRL_Measure_FLAG = false;				// Uses testMode_Measure instead of reading from ADC
BOOL testMode_SwitchOnCTRL_Task_FLAG = false; 				// Stores the PSUs waiting to be switched on in remainingPSUs[20]

// Testing Values
WORD testMode_Measure = 0;									// Used to simulate values. Recommended to be used with VoltORCurrToADCCounts()
int testMode_psu_initializationTimer[PSU_NUMBER];			// Records the real initializaionTimer values for the PSUs in switchONTask()

// Semaphores
OS_SEM monitorSem;											// Timed Interruption posts to it, monitorTask pends on it
OS_SEM seqMode_MonitorCTRLTask_Sem;

// testMode SwitchON
extern BYTE monitorSamplFunc;								// FUNCTION_PSU_VOLTAGE, FUNCTION_PSU_CURRENT, FUNCTION_SnI_VOLTAGE
extern int monitorNum;										// 0 to 11 for PSUs, 0 to 13 for SnIs


//=====================================================================================================//
//================================== PUBLIC CONFIGURATION METHODS =====================================//
//=====================================================================================================//

//==========================================INITIALIZATION=============================================//
int allSemInit (void){
	return (OSSemInit(& monitorSem,0) && OSSemInit(& seqMode_MonitorCTRLTask_Sem,0));
}


//==========================================CONFIGURATION==============================================//
//-------------------------------------------------------------------------------------------------------
// config_alarmUpdatePeriod_x50MS - configures the alarm update period
// 		#INPUT:	int newPeriodx50MS - new period to be configured
// 		#OUTPUT: None
//		>E.G.: config_alarmUpdatePeriod_x50MS ( 20 );
//-------------------------------------------------------------------------------------------------------
void config_alarmUpdatePeriod_x50MS ( int newPeriodx50MS ){
	if (newPeriodx50MS<50){	alarmUpdatePeriodx50MS = newPeriodx50MS;}
	else{iprintf(" ERROR - CONFIG: Value for newPeriodMS (%d) (newPeriod<50)\n", newPeriodx50MS);}
}


//-------------------------------------------------------------------------------------------------------
// config_AlarmCTRLUpdate_SnI - Includes/excludes SnI voltage alarms when using updateAlarms(void)
// 		#INPUT:	BOOL sniUpdateFLAG - TRUE(default): include. FALSE: exclude.
// 		#OUTPUT: None
//		>E.G.: config_AlarmCTRLUpdate_SnI ( true );
//-------------------------------------------------------------------------------------------------------
void config_AlarmCTRLUpdate_SnI ( BOOL sniUpdateFLAG ){
	config_AlarmCTRLUpdate_SnI_FLAG = sniUpdateFLAG;
}


//-------------------------------------------------------------------------------------------------------
// config_MonitorCTRL_SnI - Include/exclude SnI voltages in monitorTask
// 		#INPUT:	BOOL monitorSnI - TRUE(default): include. FALSE: exclude.
// 		#OUTPUT: None
//		>E.G.: config_MonitorCTRL_SnI ( true );
//-------------------------------------------------------------------------------------------------------
void config_MonitorCTRL_SnI ( BOOL monitorSnI )	{
	config_MonitorCTRL_SnI_FLAG = monitorSnI;
}


//-------------------------------------------------------------------------------------------------------
// config_consoleOutput_RDAC - Toggles output messages of contents of buffers and results of using
//								RDAC functions from RDACLibrary
// 		#INPUT:	BOOL consoleOut - TRUE: output messages. FALSE(default): No output.
// 		#OUTPUT: None
//		>E.G.: config_consoleOutput_RDAC ( ON );
//-------------------------------------------------------------------------------------------------------
void config_consoleOutput_RDAC ( BOOL consoleOut )	{
	consoleOutputRDAC = consoleOut;
	printf("     #RDAC Output toggled %s#\n", (consoleOutputRDAC?"ON":"OFF"));
}


//-------------------------------------------------------------------------------------------------------
// config_consoleOutput_I2CnSPI - Toggles output messages of contents of buffers and results of using
//								I2C and SPI functions from I2C&SPILibrary
// 		#INPUT:	BOOL consoleOut - TRUE: output messages. FALSE(default): No output.
// 		#OUTPUT: None
//		>E.G.: config_consoleOutput_I2CnSPI ( ON );
//-------------------------------------------------------------------------------------------------------
void config_consoleOutput_I2CnSPI ( BOOL consoleOut )	{
	consoleOutputI2C = consoleOut;
	iprintf("     #I2C & SPI Output toggled %s#\n", (consoleOutputI2C?"ON":"OFF"));
}



//===========================================PAUSE TASKS===============================================//
//-------------------------------------------------------------------------------------------------------
// iddleMode_AlarmCTRL_Task - Stop alarm task from updating alarms(normalMode), resetting them instead (idleMode)
// 		#INPUT:	BOOL iddleModeAlarmCTRLFLAG - 	TRUE: alarms are reseted (iddleMode).
//												FALSE(default): alarms are updated (normalMode)
// 		#OUTPUT: None
//		>E.G.: iddleMode_AlarmCTRL_Task ( true );
//-------------------------------------------------------------------------------------------------------
void iddleMode_AlarmCTRL_Task ( BOOL iddleModeAlarmCTRLFLAG ){
	idleMode_AlarmCTRL_Task_FLAG = iddleModeAlarmCTRLFLAG;
//	iprintf("idleMode_AlarmCTRL %s\n", (iddleModeAlarmCTRLFLAG?"ON":"OFF"));
}

//-------------------------------------------------------------------------------------------------------
// sequentialMode_MonitorCTRL_Task - Stops monitorTask from monitoring voltages & currents (normalMode),
//				pending the task on a semaphore and executing it once every time the method is called
//				(sequentialMode).
// 		#INPUT:	BOOL seqModeMonitorCTRLFLAG - 	TRUE: monitorTask pends on a semaphore, and posts (sequentialMode).
//												FALSE(default): monitorTask works automatically (normalMode)
// 		#OUTPUT: None
//		>E.G.: sequentialMode_MonitorCTRL_Task ( true );
//-------------------------------------------------------------------------------------------------------
void sequentialMode_MonitorCTRL_Task ( BOOL seqModeMonitorCTRLFLAG ){
	seqMode_MonitorCTRL_Task_FLAG = seqModeMonitorCTRLFLAG;
	OSSemPost(&seqMode_MonitorCTRLTask_Sem);
	OSTimeDly(1);		// Wait for MonitorTask to stop at sequentialMode Semaphore
	//Pins[26]=seqModeMonitorCTRLFLAG;
}


//============================================TEST MODES===============================================//
//-------------------------------------------------------------------------------------------------------
// testMode_MonitorCTRL_Measure - Controls the measure input for monitorTask, simulating a value when ON
// 		#INPUT:	BOOL testModeMonitorCTRLMeasureFLAG - 	TRUE: measure using testMode_Measure (testMode).
//														FALSE(default): measure from a2d module (normalMode)
// 		#OUTPUT: None
//		>E.G.: iddleMode_AlarmCTRL_Task ( true );
//-------------------------------------------------------------------------------------------------------
void testMode_MonitorCTRL_Measure ( BOOL testModeMonitorCTRLMeasureFLAG ){
	testMode_MonitorCTRL_Measure_FLAG = testModeMonitorCTRLMeasureFLAG;
}

//-------------------------------------------------------------------------------------------------------
// testMode_set_Measure - Sets a value to the simulated measure when monitorTask is in sequentialMode
// 		#INPUT:	 WORD testMeasure - Use the lower 12 bits for a simulated measure.
// 		#OUTPUT: None
//		>E.G.: testMode_set_Measure ( 0x063A );
//-------------------------------------------------------------------------------------------------------
void testMode_set_Measure ( WORD testMeasure ){
	testMode_Measure = testMeasure;
}


//-------------------------------------------------------------------------------------------------------
// testMode_SwitchOnCTRL_Task - Controls console output in switchONPSUsTask
// 		#INPUT:	 BOOL testModeSwitchOnCTRLTaskFLAG - 	TRUE: iprintf output of switchONTask (testMode).
//														FALSE(default): no output of switchONTask (normalMode)
// 		#OUTPUT: None
//		>E.G.: testMode_SwitchOnCTRL_Task ( true );
//-------------------------------------------------------------------------------------------------------
void testMode_SwitchOnCTRL_Task ( BOOL testModeSwitchOnCTRLTaskFLAG ){
	testMode_SwitchOnCTRL_Task_FLAG = testModeSwitchOnCTRLTaskFLAG;
	monitorSamplFunc = FUNCTION_PSU_VOLTAGE;
	monitorNum = 0;
}

