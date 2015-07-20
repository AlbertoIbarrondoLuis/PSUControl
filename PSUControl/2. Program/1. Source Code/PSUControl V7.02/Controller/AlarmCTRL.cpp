/*
 * AlarmCTRL.cpp
 *
 *	alarmTask - RTOS task executed once every 100ms that checks the voltage & current of
 *	all the PSUs and triggers alarms if a limit is reached, as well as being in charge of the
 *	buttonTask (control of manual button)
 *
 *  Created on: 07-may-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Controller/Controller.h"


//===========================================VARIABLES=================================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];			// MAIN PSU ARRAY LIST
extern SnI_TYPE sniList[INT_VCC_n12V + 1];		// Supply & Internal voltages List

// Selection Lists
extern BOOL psuSelectionList[PSU_NUMBER];		// Used for various functions where multiple psu selection is required.
extern BOOL sniSelectionList[SnI_NUMBER];		// Used for various functions where multiple sni selection is required.
extern BOOL alarmSelectionList[ALARM_NUMBER];	// Used for various functions where multiple alarm selection is required.

// Timing
extern int alarmUpdatePeriodx50MS;				// Period x50ms for alarm update task

// Configuration
extern BOOL config_AlarmCTRLUpdate_SnI_FLAG;	// Configure whether SnI alarms are updated or not in updateAlarms()

// Testing
extern BOOL idleMode_AlarmCTRL_Task_FLAG;		// TRUE: alarms aren't updated (iddleMode). FALSE: regular alarm update

// Auxiliary
uint NumAlm, kAlm;
BOOL psuProtocolList [PSU_NUMBER];

//=====================================================================================================//
//=====================================  PUBLIC ALARM METHODS  ========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// alarmTask - Independent process in charge of monitoring & updating all the PSU alarms once every 100ms
//				(default) and executing the programmed protocols if an alarm is triggered.
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: OSSimpleTaskCreate( alarmTask, ALARM_PRIO ); - initialized in main.cpp
//-------------------------------------------------------------------------------------------------------
void alarmTask (void *p){
	while (true){ // loop forever
		OSTimeDly(alarmUpdatePeriodx50MS);   	// Critical, otherwise the lower tasks wont receive computing time.
		if(!idleMode_AlarmCTRL_Task_FLAG){
			updateAlarms();						// Update all the alarms
		}
		else{
			resetAlarms(); 						// Reset all alarms while in idle Mode
		}
		buttonTask();							// Processes the button
	}
}


//-------------------------------------------------------------------------------------------------------
// resetAlarms - Sets to 0 the values of alarmCounters, alarmStatus and alarmLimitReached for the selected
//					alarms, for the selected PSUs/SnIs.
// 		#INPUT:+BOOL Selection[] - An array of booleans, storing the selected PSUs, SnIs
//				BOOL alarmSelection[ALARM_NUMBER] - An array of booleans, storing the selected alarms
// 				BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
// 			   +None: it will reset all the alarms.
// 		#OUTPUT: None
//		>E.G.: resetAlarms(psuSelectionList, alarmSelectionList, PSU_TYPE_LIST); - used in resetAlarms()
//-------------------------------------------------------------------------------------------------------
void resetAlarms ( void ) {	memset(psuSelectionList, 1, sizeof(psuSelectionList)); // Resets All the alarms
							memset(alarmSelectionList, 1, sizeof(alarmSelectionList));
							memset(sniSelectionList, 1, sizeof(sniSelectionList));
							resetAlarms(psuSelectionList, alarmSelectionList, PSU_TYPE_LIST);
							resetAlarms(sniSelectionList, alarmSelectionList, SnI_TYPE_LIST);}
void resetAlarms ( BOOL Selection[] ,  BOOL alarmSelection[ALARM_NUMBER] , BOOL psu_sni){
	uint k, g;
	if (psu_sni==PSU_TYPE_LIST){
		for (k=0; k<PSU_NUMBER; k++){
			if ( Selection[k] ){
				for (g=0; g<ALARM_NUMBER; g++){
					if(alarmSelection[g]){
						psuList[k].alarmCounters[g] = 0;
						psuList[k].alarmStatus[g] = false;
						psuList[k].alarmLimitReached[g]=false;
					}
				}
			}
		}
	}
	else{ // psu_sni==SnI_TYPE_LIST
		for (k=0; k<SnI_NUMBER; k++){
			if ( Selection[k] ){
				for (g=0; g<ALARM_NUMBER-2; g++){
					if(alarmSelection[g]){
						sniList[k].alarmCounters[g] = 0;
						sniList[k].alarmStatus[g] = false;
						sniList[k].alarmLimitReached[g]=false;
					}
				}
			}
		}
	}
}


//-------------------------------------------------------------------------------------------------------
// toggleAlarms - Sets to 0 the values of alarmCounters, alarmStatus and alarmLimitReached for the selected
//					alarms, for the selected PSUs/SnIs.
// 		#INPUT:+BOOL almWatch - ON/OFF, sets all the alarms into Connected/Disconnected status
//			   +BOOL Selection[] - An array of booleans, storing the selected PSUs, SnIs
//				BOOL alarmSelection[ALARM_NUMBER] - An array of booleans, storing the selected alarms
// 				BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
//				BOOL almWatch - ON/OFF, sets the selected alarms into Connected/Disconnected status
// 		#OUTPUT: None
//		>E.G.: resetAlarms(psuSelectionList, alarmSelectionList, PSU_TYPE_LIST); - used in resetAlarms()
//-------------------------------------------------------------------------------------------------------
void toggleAlarms (  BOOL almWatch  ) {	memset(psuSelectionList, 1, sizeof(psuSelectionList));
										memset(alarmSelectionList, 1, sizeof(alarmSelectionList));
										memset(sniSelectionList, 1, sizeof(sniSelectionList));
										toggleAlarms(psuSelectionList, alarmSelectionList, PSU_TYPE_LIST, almWatch);
										toggleAlarms(sniSelectionList, alarmSelectionList, SnI_TYPE_LIST, almWatch);}
void toggleAlarms ( BOOL Selection[] ,  BOOL alarmSelection[ALARM_NUMBER] , BOOL psu_sni, BOOL almWatch ){
	uint k; uint g;
	if (psu_sni==PSU_TYPE_LIST){
		for (k=0; k<PSU_NUMBER; k++){
			if ( Selection[k] ){
				for (g=0; g<ALARM_NUMBER; g++){
					if(alarmSelection[g]){
						psuList[k].alarmWatch[g]=almWatch;
					}
				}
			}
		}
	}
	else{ // psu_sni==SnI_TYPE_LIST
		for (k=0; k<SnI_NUMBER; k++){
			if ( Selection[k] ){
				for (g=0; g<ALARM_NUMBER-2; g++){
					if(alarmSelection[g]){
						sniList[k].alarmWatch[g]=almWatch;
					}
				}
			}
		}
	}
}


//-------------------------------------------------------------------------------------------------------
// setAlarm - Sets both the value and the time for the desired alarm, and connects the alarm (alarmWatch)
// 		#INPUT:	BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
// 				int Num - number of PSU (0 to 11) or SnI (0 to 13) to be checked
//				BOOL inf_sup - INFERIOR/SUPERIOR, select the limit (lower/upper) to be checked
// 				BOOL volt_corr - VOLTAGE/CURRENT, select type of magnitude
//				float value - alarm limit value
// 				int time - alarm limit times until alarm is triggered
// 		#OUTPUT: None
//		>E.G.: setAlarm (PSU_TYPE_LIST, 10, INFERIOR, CURRENT, -15.6, 20)
//-------------------------------------------------------------------------------------------------------
void setAlarm ( BOOL psu_sni, int num, BOOL inf_sup, BOOL volt_curr, float value, int time){
	if(psu_sni==PSU_TYPE_LIST){
		psuList[num].alarmLimitTimes[_(inf_sup, volt_curr)]=time;
		psuList[num].alarmLimitValues[_(inf_sup, volt_curr)]=value;
		psuList[num].alarmWatch[_(inf_sup, volt_curr)]=true;
	}
	else{ // psu_sni==SnI_TYPE_LIST
		sniList[num].alarmLimitTimes[_(inf_sup, volt_curr)]=time;
		sniList[num].alarmLimitValues[_(inf_sup, volt_curr)]=value;
		sniList[num].alarmWatch[_(inf_sup, volt_curr)]=true;
	}
}





//=====================================================================================================//
//====================================  PRIVATE ALARM METHODS  ========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// updateAlarms - Takes a sample of Voltage and Current for all the PSUs and updates
// 				  their alarm status, counters and flags by comparing the sample with
//				  the programmed limit
// 		#INPUT:	None
// 		#OUTPUT: None
//		>E.G.: updateAlarms(); - used in alarmTask()
//-------------------------------------------------------------------------------------------------------
void updateAlarms (void){
	// Goes over each PSU in psuList
	for (NumAlm=0; NumAlm<PSU_NUMBER; NumAlm++){
		// Checks each alarm only if the PSU is ON
		if (psuList[NumAlm].psuStatus==ON){
			alarmCheck(NumAlm, INFERIOR, VOLTAGE, PSU_TYPE_LIST);
			alarmCheck(NumAlm, SUPERIOR, VOLTAGE, PSU_TYPE_LIST);
			alarmCheck(NumAlm, INFERIOR, CURRENT, PSU_TYPE_LIST);
			alarmCheck(NumAlm, SUPERIOR, CURRENT, PSU_TYPE_LIST);
		}
	}

	//Same procedure for sniList, controlled by config_AlarmCTRLUpdate_SnI_FLAG (switched ON by default)
	if (config_AlarmCTRLUpdate_SnI_FLAG){
		for (NumAlm=0; NumAlm<SnI_NUMBER; NumAlm++){
			alarmCheck(NumAlm, INFERIOR, VOLTAGE, SnI_TYPE_LIST);
			alarmCheck(NumAlm, SUPERIOR, VOLTAGE, SnI_TYPE_LIST);
		}
	}
}


//-------------------------------------------------------------------------------------------------------
// alarmCheck - Checks a singular alarm status for the selected PSU/SnI and updates its status based on
//				the last received measure.
// 		#INPUT:	int Num - number of PSU (0 to 11) or SnI (0 to 13) to be checked
//				BOOL inf_sup - INFERIOR/SUPERIOR, select the limit (lower/upper) to be checked
// 				BOOL volt_corr - VOLTAGE/CURRENT, select type of magnitude
// 				BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
// 		#OUTPUT: None
//		>E.G.: alarmCheck(psuNum, INFERIOR, VOLTAGE, PSU_TYPE_LIST); - used in updateAlarms()
//-------------------------------------------------------------------------------------------------------
void alarmCheck (int Num, BOOL inf_sup, BOOL volt_corr, BOOL psu_sni){
	if (psu_sni == PSU_TYPE_LIST){		// Checking a PSU_TYPE from psuList
		if(psuList[Num].alarmWatch[_(inf_sup,volt_corr)]){				// If the selected alarm is being watched (INFERIOR/SUPERIOR, VOLTAGE/CURRENT)
																					// Refresh alarmLimitReached value
			if   (inf_sup==INFERIOR) {psuList[Num].alarmLimitReached[_(inf_sup,volt_corr)]=((volt_corr==VOLTAGE?psuList[Num].vOut:psuList[Num].cOut) <= psuList[Num].alarmLimitValues[_(inf_sup,volt_corr)]);}
			else/*inf_sup==SUPERIOR*/{psuList[Num].alarmLimitReached[_(inf_sup,volt_corr)]=((volt_corr==VOLTAGE?psuList[Num].vOut:psuList[Num].cOut) >= psuList[Num].alarmLimitValues[_(inf_sup,volt_corr)]);}
			if (psuList[Num].alarmLimitReached[_(inf_sup,volt_corr)]){			// If limit is exceeded
				psuList[Num].alarmCounters[_(inf_sup,volt_corr)]++;					// Increment Alarm Counter

				if (psuList[Num].alarmCounters[_(inf_sup,volt_corr)]>=psuList[Num].alarmLimitTimes[_(inf_sup,volt_corr)]){
																						//  Trigger on the alarm if counter reaches the time limit
					if (psuList[Num].alarmStatus[_(inf_sup,volt_corr)] == false ){			// If it's the first time the alarm is triggered, execute protocols
						psuList[Num].alarmStatus[_(inf_sup,volt_corr)] = true;
						BYTE j;
						for (j=0; j<PROTOCOLS_NUMBER; j++){									// Goes over each alarm protocol for the activated alarm
							if (psuList[Num].alarmProtocols[___(inf_sup,volt_corr, j)]){
								executeAlarmProtocol (Num, inf_sup,volt_corr, j, psu_sni);
							}
						}
					}
					psuList[Num].alarmCounters[_(inf_sup,volt_corr)]=psuList[Num].alarmLimitTimes[_(inf_sup,volt_corr)];
				}																		// Sets the counter to time limit (thus preventing it to increase further)
			}
			else{																	// If limit is not exceeded
				if (psuList[Num].alarmCounters[_(inf_sup,volt_corr)] > 0){			// And counter is higher than 0
					psuList[Num].alarmCounters[_(inf_sup,volt_corr)] -= 1;			// Decrease counter until it reaches 0

				}
				if (psuList[Num].alarmCounters[_(inf_sup,volt_corr)] == 0){			// And when it hits 0, shut off the alarm.
					psuList[Num].alarmStatus[_(inf_sup,volt_corr)] = false;
				}
			}
		}
		else{
			psuList[Num].alarmCounters[_(inf_sup,volt_corr)] = 0;
			psuList[Num].alarmStatus[_(inf_sup,volt_corr)] = false;
		}
	}

	else{			// psu_sni == SnI_TYPE_LIST - Checking an SnI_TYPE from sniList
		if(sniList[Num].alarmWatch[_(inf_sup,volt_corr)]){				// If the selected alarm is being watched (INFERIOR/SUPERIOR, VOLTAGE/CURRENT)
																					// Refresh alarmLimitReached value
			if(inf_sup == INFERIOR){
				sniList[Num].alarmLimitReached[_(inf_sup,volt_corr)]=(sniList[Num].vOut <= sniList[Num].alarmLimitValues[_(inf_sup,volt_corr)]);
			}
			else{
				sniList[Num].alarmLimitReached[_(inf_sup,volt_corr)]=(sniList[Num].vOut >= sniList[Num].alarmLimitValues[_(inf_sup,volt_corr)]);
			}
			if (sniList[Num].alarmLimitReached[_(inf_sup,volt_corr)]){			// If limit is exceeded
				sniList[Num].alarmCounters[_(inf_sup,volt_corr)]++;					// Increment Alarm Counter

				if (sniList[Num].alarmCounters[_(inf_sup,volt_corr)]>=sniList[Num].alarmLimitTimes[_(inf_sup,volt_corr)]){
																						//  Trigger on the alarm if counter reaches the time limit
					if (sniList[Num].alarmStatus[_(inf_sup,volt_corr)] == false ){			// If it's the first time the alarm is triggered, execute protocols
						sniList[Num].alarmStatus[_(inf_sup,volt_corr)] = true;
						BYTE j;
						for (j=0; j<PROTOCOLS_NUMBER; j++){									// Goes over each alarm protocol for the activated alarm
							if (sniList[Num].alarmProtocols[___(inf_sup,volt_corr, j)]){
								executeAlarmProtocol (Num, inf_sup,volt_corr, j, psu_sni);
							}
						}
					}
					sniList[Num].alarmCounters[_(inf_sup,volt_corr)]=sniList[Num].alarmLimitTimes[_(inf_sup,volt_corr)];
				}																		// Sets the counter to time limit (thus preventing it to increase further)
			}
			else{																	// If limit is not exceeded
				if (sniList[Num].alarmCounters[_(inf_sup,volt_corr)] > 0){			// And counter is higher than 0
					sniList[Num].alarmCounters[_(inf_sup,volt_corr)] -= 1;			// Decrease counter until it reaches 0

				}
				if (sniList[Num].alarmCounters[_(inf_sup,volt_corr)] == 0){			// And when it hits 0, shut off the alarm.
					sniList[Num].alarmStatus[_(inf_sup,volt_corr)] = false;
				}
			}
		}
		else{
			sniList[Num].alarmCounters[_(inf_sup,volt_corr)] = 0;
			sniList[Num].alarmStatus[_(inf_sup,volt_corr)] = false;
		}
	}
}


//-------------------------------------------------------------------------------------------------------
// executeAlarmProtocol - Carries out a different protocol depending of the type of alarm
//						 (SUPERIOR/INFERIOR,VOLTAGE/CURRENT).
// 		#INPUT:	int Num - number of PSU (0 to 11) or SnI (0 to 13) to be checked
//				BOOL inf_sup - INFERIOR/SUPERIOR, select the protocol (lower/upper) to be executed
// 				BOOL volt_corr - VOLTAGE/CURRENT, select type of magnitude
//				BYTE protocolNum - PROTOCOL_SHUTDOWN/PROTOCOL_MOD_VOLTAGE/PROTOCOL_MESSAGE
// 				BOOL psu_sni - PSU_TYPE_LIST/SnI_TYPE_LIST, to select a PSU or a SnI
// 		#OUTPUT: None
//		>E.G.: executeAlarmProtocol (4, INFERIOR,CURRENT, PROTOCOL_SHUTDOWN, PSU_TYPE_LIST);
//-------------------------------------------------------------------------------------------------------
void executeAlarmProtocol (int Num, BOOL inf_sup, BOOL volt_corr, BYTE protocolNum, BOOL psu_sni){
	switch (protocolNum){		// (0) Shut down certain PSUs, (1) Modify Voltage, (2) Send Alarm Message.
		case PROTOCOL_SHUTDOWN:
			if (psu_sni==PSU_TYPE_LIST) 	{conversionShutdownWORDtoARRAY(psuList[Num].alarmProtocolShutdown[_(inf_sup, volt_corr)], psuProtocolList);}
			else/*psu_sni==SnI_TYPE_LIST*/ 	{conversionShutdownWORDtoARRAY(sniList[Num].alarmProtocolShutdown[_(inf_sup, volt_corr)], psuProtocolList);}
			switchOFFPSUs(psuProtocolList);
			break;
		case PROTOCOL_MOD_VOLTAGE:
			if (psu_sni==PSU_TYPE_LIST) 	{adjustRdac(Num, psuList[Num].alarmProtocolVoltage[_(inf_sup,volt_corr)]);}
			break;
		case PROTOCOL_MESSAGE:
			iprintf(ALARM_MESSAGE);
			//TODO: Maybe add some other alarm output - left for future implementation
			break;
	}
}

