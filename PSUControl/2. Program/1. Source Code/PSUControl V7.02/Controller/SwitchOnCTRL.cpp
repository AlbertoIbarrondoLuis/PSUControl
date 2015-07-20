/*
 * SwitchOnCTRL.cpp
 *
 *	Relay connection & disconnection, delayed switch on process (based on initializationTimer) and
 *	programmed voltage update. It also sets PSUs into low voltage when disconnected, minimizing
 *	power consumption
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Controller/Controller.h"

//==============================================VARIABLES==============================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];			// Power supply units' array list
extern BOOL psuSelectionList[PSU_NUMBER];		// Used for several functions where multiple psu selection is required.

// Switching ON
BOOL powerONProcess = false; 					// used to initialize the system.
int powerONticks=0; 							// delay counter when initializing the system.
BOOL psuSelectionListPending [PSU_NUMBER] = {0,0,0,0,0,0,0,0,0,0,0,0};		// bits in TRUE are PSUs pending to be switched on

// Testing
extern BOOL testMode_SwitchOnCTRL_Task_FLAG;
extern int testMode_psu_initializationTimer[PSU_NUMBER];

// Led Out
BOOL ledOUT = OFF;

// Button
extern BOOL buttonActionFLAG;
BOOL buttonStatusFLAG = ledOUT;					// System is ON/OFF
BOOL psuBUTTONSelectionList[PSU_NUMBER];		// Stores the PSUs with status ON as TRUE in its corresponding
												// bit, and as FALSE with status OFF
// Pause Tasks
extern BOOL idleMode_AlarmCTRL_Task_FLAG;		// Sets Alarm Task into idle mode
extern BOOL seqMode_MonitorCTRL_Task_FLAG;		// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem

// Auxiliary
int but_psuNum;
BOOL idleModeAlarmTask_SwitchOn, seqModeMonitorTask_SwitchOn;

//=====================================================================================================//
//================================ PUBLIC PSUs SWITCH ON/OFF METHODS ==================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// switchONPSUsTask - Manages the complete PSU switching-on process for the selected PSUs: Initializes
//				its RAM values, updates its RDAC Register values, tells the switchONPSUsTask to
//				begin the delayed switching-on process.
// 		#INPUT: +None: all the PSUs selected
//				+BOOL psuSelection[PSU_NUMBER] - boolean array where ONES are PSUs being switched on
// 		#OUTPUT: None
//		>E.G.:	switchONPSUs(selectedNums); - used in switchOnOffINFC ();
//-------------------------------------------------------------------------------------------------------
void switchONPSUs( void ){ memset(psuSelectionList, 1, PSU_NUMBER); switchONPSUs(psuSelectionList);}
void switchONPSUs( BOOL psuSelection[PSU_NUMBER] ){
	powerONProcess=true;					// Beginning of the process

											// Stop alarmTask and monitorTask
	idleModeAlarmTask_SwitchOn = idleMode_AlarmCTRL_Task_FLAG;
	seqModeMonitorTask_SwitchOn = seqMode_MonitorCTRL_Task_FLAG;
	iddleMode_AlarmCTRL_Task(ON);
	sequentialMode_MonitorCTRL_Task(ON);

	initializeValuesPSUsSnIs();				// Load psuList values from RAM or set them to default

	updateVoltagePSUs( psuSelection );		// Sets the PSUs' potentiometers (RDACS) to the loaded/default values

	switchONPSUsTask( psuSelection );		// Task to initialize PSUs with certain delay (initializationTimer)

											// Sets alarmTask and monitorTask to previous mode
	iddleMode_AlarmCTRL_Task(idleModeAlarmTask_SwitchOn);
	sequentialMode_MonitorCTRL_Task(seqModeMonitorTask_SwitchOn);

    powerONProcess=false;					// End of the process
}


//-------------------------------------------------------------------------------------------------------
// switchOFFPSUs - Disconnects the selected PSUs' relays, sets their power to minimum, and updates LED OUT
// 		#INPUT: +None: all the PSUs selected
//				+BOOL psuSelection[PSU_NUMBER] - boolean array where ONES are PSUs being switched off
// 		#OUTPUT: None
//		>E.G.:	switchOFFPSUs(psuONSelectionList); - used in Interruption.cpp;
//-------------------------------------------------------------------------------------------------------
void switchOFFPSUs( void ){ memset(psuSelectionList, true, sizeof(psuSelectionList)); switchOFFPSUs(psuSelectionList);}
void switchOFFPSUs( BOOL psuSelection[PSU_NUMBER] ){
	int psuNum;			BOOL allPsusOFF = true;			// used to check if all psus are disconnected

														// Stop alarmTask and monitorTask
	idleModeAlarmTask_SwitchOn = idleMode_AlarmCTRL_Task_FLAG;
	seqModeMonitorTask_SwitchOn = seqMode_MonitorCTRL_Task_FLAG;
	iddleMode_AlarmCTRL_Task(ON);
	sequentialMode_MonitorCTRL_Task(ON);

	saveInFlashValuesPSUsSNIs();						// Save Values in Flash Memory

	for (psuNum=0; psuNum<PSU_NUMBER; psuNum++){
		if(psuSelection[psuNum]){
			disconnectPSU ( psuNum );
			adjustRdac(psuNum, INITIAL_VOLTAGE);		// minimize power consumption when idle
		}
		allPsusOFF &= (psuList[psuNum].psuStatus==OFF);	// Check each PSU status
	}
	LED_OUT_ON=!allPsusOFF;								// If all are disconnected, LED OUT is set OFF

														// Sets alarmTask and monitorTask to previous m
	iddleMode_AlarmCTRL_Task(idleModeAlarmTask_SwitchOn);
	sequentialMode_MonitorCTRL_Task(seqModeMonitorTask_SwitchOn);
}



//==============================================BUTTON=================================================//
//-------------------------------------------------------------------------------------------------------
// buttonTask - Switches ON/OFF the PSUs when an IRQ1 is triggered (setting buttonAction = true)
// 		#INPUT:	None
// 		#OUTPUT: None
//		>E.G.: buttonTask (); - called in alarmTask()
//-------------------------------------------------------------------------------------------------------
void buttonTask ( void ){
	if (buttonActionFLAG){
		if(buttonStatusFLAG==ON){
			for (but_psuNum=0; but_psuNum<PSU_NUMBER; but_psuNum++){
				psuBUTTONSelectionList[but_psuNum] = (getPSU(but_psuNum).psuStatus == ON);
			}
			switchOFFPSUs(psuBUTTONSelectionList);// Turns OFF all the PSUs what were ON (psuStatus == ON).
			iddleMode_AlarmCTRL_Task(ON);
			buttonStatusFLAG = OFF;
		}
		else{ // buttonStatusFLAG==OFF
			switchONPSUs(psuBUTTONSelectionList); // Turns ON all the PSUs what were turned off when button was pressed the first time.
			iddleMode_AlarmCTRL_Task(OFF);
			buttonStatusFLAG = ON;
		}
		buttonActionFLAG = false;
	}
}




//=====================================================================================================//
//================================ PRIVATE PSUs SWITCH ON/OFF METHODS =================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// switchONPSUsTask - Task in charge of the PSUs' delayed switching-on process, connecting the relays
//					  when each PSU reaches its initializationTimer. The task is endedWhen every PSU has
//					  been connected.
// 		#INPUT: +BOOL psuSelection[PSU_NUMBER] - boolean array where ONES are PSUs being switched on
// 		#OUTPUT: None
//		>E.G.:	switchONPSUsTask( psuSelection ); - used in switchONPSUs();
//-------------------------------------------------------------------------------------------------------
void switchONPSUsTask( BOOL psuSelection[PSU_NUMBER] ){
	powerONticks = 0; memcpy(psuSelectionListPending, psuSelection, PSU_NUMBER);
	int psuNum; BOOL allConnected = false;
	if (testMode_SwitchOnCTRL_Task_FLAG){
		iprintf("powerONticks = %d  -  PSUs pending: ", powerONticks);
		printBuffer(psuSelectionListPending, PSU_NUMBER);iprintf("\n");
	}
	while(!allConnected){
		allConnected = true;
		for (psuNum=0; psuNum<PSU_NUMBER; psuNum++){
			if(psuSelectionListPending[psuNum]){
				if(psuList[psuNum].initializationTimer == powerONticks){
					psuList[psuNum].ReadyToConnect = true;
					connectPSU(psuNum);
					psuSelectionListPending[psuNum] = false;
					if(testMode_SwitchOnCTRL_Task_FLAG){ // status storage if testMode ON
						testMode_psu_initializationTimer[psuNum]= powerONticks;
					}
				}
				else {
					allConnected = false;
				}
			}
		}
		if (testMode_SwitchOnCTRL_Task_FLAG){ // console output if testMode ON
			iprintf("powerONticks = %d  -  PSUs pending: ", powerONticks);
			printBuffer(psuSelectionListPending, PSU_NUMBER);iprintf("\n");
		}
		powerONticks++;
		OSTimeDly(TICKS_100MS);
	}
}


//-------------------------------------------------------------------------------------------------------
// connectPSU - Connects the selected PSU by switching its Relay and activates LED OUT
// 		#INPUT: int psuNum - number of PSU (0 to 11) whose Relay will be connected
// 		#OUTPUT: None
//		>E.G.:	connectPSU(int psuNum); - used in switchONPSUsTask();
//-------------------------------------------------------------------------------------------------------
void connectPSU ( int psuNum ){
	connectRelay ( psuNum );
	psuList[psuNum].psuStatus=true;
	psuList[psuNum].ReadyToConnect=false;
	LED_OUT_ON= true;		// LED OUT connected when any PSU is ON
}


//-------------------------------------------------------------------------------------------------------
// disconnectPSU - Disconnects the selected PSU's relay
// 		#INPUT: int psuNum - number of PSU (0 to 11) whose Relay will be disconnected
// 		#OUTPUT: None
//		>E.G.:	disconnectPSU(int psuNum); - used in switchOFFPSUs();
//-------------------------------------------------------------------------------------------------------
void disconnectPSU(int psuNum){
	disconnectRelay ( psuNum );
	psuList[psuNum].psuStatus=false;
	psuList[psuNum].ReadyToConnect=false;
							// LED OUT disconnection is controlled by switchOFFPSUs
}


