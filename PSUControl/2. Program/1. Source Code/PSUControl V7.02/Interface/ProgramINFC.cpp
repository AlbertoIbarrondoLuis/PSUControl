/*
 * ProgramINFC.cpp
 *
 *	Switch On/Off PSUs, change their output voltages and configure all the alarms (both PSUs and SnIs)
 *
 *  Created on: 01/06/2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface.h"

//===========================================VARIABLES=================================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];
extern SnI_TYPE sniList[SnI_NUMBER];

// Keyboard
extern char categorySelectionINFC;
char programSelectionDISP;
static char progChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD progChar = sgetchar(0);

// Selection arrays
BOOL selectedNums[SnI_NUMBER] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};	// Select PSUs/SnIs
BOOL selectedAlarms[4] = {0,0,0,0};								// Select Alarms
BOOL selectedProtocols[3] = {0,0,0};							// Select Alarm Protocols
BOOL selectedShutdown[PSU_NUMBER] = {0,0,0,0,0,0,0,0,0,0,0,0};	// Select PSUs for Protocol Shutdown

// Pause Tasks
extern BOOL idleMode_Alarm;
extern BOOL seqMode_Monitor;


//=====================================================================================================//
//===================================== PUBLIC PROGRAM MENU ===========================================//
//=====================================================================================================//

void ProgramINFC ( void ){
	// Setting alarm and monitor Tasks into Pause status
	if (!idleMode_Alarm)	{ iddleMode_AlarmCTRL_Task (true);		}
	if (!seqMode_Monitor)	{ sequentialMode_MonitorCTRL_Task(true);}

	ERASE_CONSOLE
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("-------------------------------- PROGRAM MENU ---------------------------------\r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("Power Supply Units (PSUs):\n" );
	iprintf(" (1) Switch ON/OFF\r\n" );
	iprintf(" (2) Change Output Voltage\r\n" );
	iprintf(" (3) Connect/Disconnect Alarms\r\n" );
	iprintf(" (4) Configure Alarms\r\n" );
	iprintf(" (5) Initialization Timer\r\n" );
	iprintf(" (+) Default Values\r\n" );
	iprintf("Supply and Internal Voltages (SnIs):\n" );
	iprintf(" (6) Configure Alarms\n" );
	iprintf(" (7) Connect/Disconnect Alarms\r\n" );
	iprintf(" (-) Default Values\r\n" );
	iprintf("\n (e) EXIT TO GENERAL MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\nEnter command:  " );
	categorySelectionINFC = sgetchar( 0 );  iprintf("%c\n", categorySelectionINFC);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
	processCommandProgramINFC();
}





//=====================================================================================================//
//==================================== PRIVATE PROGRAM METHODS ========================================//
//=====================================================================================================//

// Choose Program
void processCommandProgramINFC ( void ){
	switch (categorySelectionINFC){
		case '1': // Switch ON/OFF PSUs
			iprintf(" (1) Switch ON/OFF\r\n" ); WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			switchOnOffINFC();
			break;
		case '2': // Change Output Voltage PSUs
			iprintf(" (2) Change Output Voltage\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			changeOutputVoltageINFC();
			break;
		case '3': // Connect/Disconnect Alarms PSU
			iprintf(" (3) Connect/Disconnect Alarms\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			selectAlarmsINFC(PSU_TYPE_LIST);
			programAlarmWatchINFC(PSU_TYPE_LIST);
			break;
		case '4': // Configure Alarms PSUs
			iprintf(" (4) Configure Alarms\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			selectAlarmsINFC(PSU_TYPE_LIST);
			alarmProgramMenuINFC(PSU_TYPE_LIST); // ALARM PROGRAM MENU
			break;
		case '5': // Configure Alarms PSUs
			iprintf(" (5) Initialization Timer\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			programInitializationTimerINFC();
			break;
		case '6': // Connect/Disconnect Alarms SnI
			iprintf(" (6) Configure Alarms\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(SnI_TYPE_LIST);
			selectAlarmsINFC(SnI_TYPE_LIST);
			programAlarmWatchINFC(SnI_TYPE_LIST);
			break;
		case '7': // Configure Alarms SnIs
			iprintf(" (7) Connect/Disconnect Alarms\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(SnI_TYPE_LIST);
			selectAlarmsINFC(SnI_TYPE_LIST);
			alarmProgramMenuINFC(SnI_TYPE_LIST); // ALARM PROGRAM MENU
			break;
		case '+': // Default Values PSUs
			iprintf(" (+) Default Values PSUs\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(PSU_TYPE_LIST);
			programDefaultValuesINFC(PSU_TYPE_LIST);
			break;
		case '-': // Default Values SnIs
			iprintf(" (-) Default Values SnIs\r\n" );WAIT_FOR_KEYBOARD
			selectNumsINFC(SnI_TYPE_LIST);
			programDefaultValuesINFC(SnI_TYPE_LIST);
			break;
		case EXIT:
			iprintf("Exiting to INTERFACE MENU\n");
			// Setting alarm and monitor Tasks into previous status
			iddleMode_AlarmCTRL_Task (idleMode_Alarm);
			sequentialMode_MonitorCTRL_Task(seqMode_Monitor);
			break;
		default:
			iprintf( "\nINVALID KEY -> %c\r\n", categorySelectionINFC);
			iprintf( " \r\nPRESS ANY KEY TO RETURN TO PROGRAM MENU\r\n" );
			break;
	}
	WAIT_FOR_KEYBOARD
	saveInFlashValuesPSUsSNIs();
}

// (4/6) ALARM MENU (AlmMenu)
void alarmProgramMenuINFC ( BOOL psu_sni ){
	programSelectionDISP='\0';
	while (programSelectionDISP!=EXIT){
		ERASE_CONSOLE
		iprintf( "========================= ALARM PROGRAMMING MENU =============================\r\n", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs") );
		iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
		iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
		iprintf("Choose the variables to be programmed: \n");
		iprintf(" (1) Alarm Limit Times\n" );
		iprintf(" (2) Alarm Limit Values\n" );
		iprintf(" (3) Alarm Protocols to execute in alarm activation\n" );
		iprintf(" (4) Protocol Shutdown PSUs\n" );
		if(psu_sni==PSU_TYPE_LIST){iprintf(" (5) Protocol Modify Voltage\n" );}
		iprintf("\n (e) EXIT\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf("\nEnter command:  " );
		programSelectionDISP = sgetchar(0);  iprintf("%c\n", programSelectionDISP);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch (programSelectionDISP){
			case '1':	iprintf(" (1) Alarm Limit Times\n" );WAIT_FOR_KEYBOARD
						programAlarmLimitTimesINFC(psu_sni); break;
			case '2':	iprintf(" (2) Alarm Limit Values\n" );WAIT_FOR_KEYBOARD
						programAlarmLimitValuesINFC(psu_sni); break;
			case '3':	iprintf(" (3) Alarm Protocols to execute in alarm activation\n" );WAIT_FOR_KEYBOARD
						selectProtocolsINFC(psu_sni);programAlarmProtocolsINFC(psu_sni); break;
			case '4': 	iprintf(" (4) Protocol Shutdown PSUs\n" );WAIT_FOR_KEYBOARD
						selectShutdownINFC(psu_sni);programAlarmProtocolShutdownINFC(psu_sni); break;
			case '5': if(psu_sni==PSU_TYPE_LIST){
						iprintf(" (5) Protocol Modify Voltage\n" );WAIT_FOR_KEYBOARD
						programAlarmProtocolModVoltINFC();}; break;
			case EXIT: iprintf("Exiting to PROGRAM MENU\n"); break;
			default: iprintf("invalid command -> %c\n", programSelectionDISP); break;
		}
		if(programSelectionDISP!=EXIT){WAIT_FOR_KEYBOARD}
	}
}


//====================================   SELECTOR METHODS   ===========================================//

// Select PSUs/SnIs
void selectNumsINFC (BOOL psu_sni){
	char selectingNumsAUX[1] = {'\0'};
	int hexValue=0;
	memset(selectedNums, 0, sizeof selectedNums);
	while (selectingNumsAUX[0]!=' '){
		ERASE_CONSOLE
		iprintf("================================ Select %s ==================================\r\n", (psu_sni?"PSUs":"SnIs") );
		iprintf("Press the keys to toggle selected. # is selected, blank isn't.\n" );
		iprintf("Press SPACE KEY to continue, 'P' to select all, 'O' to unselect all\n" );
		if (psu_sni==PSU_TYPE_LIST){
			iprintf("\n   PSU OUTPUTS\n");
			iprintf(" (0) SF1_A + [%s]    |   (1) SF1_B + [%s]\n", (selectedNums[0]?"#":" "), (selectedNums[1]?"#":" "));
			iprintf(" (2) SF2_A + [%s]    |   (3) SF2_B + [%s]\n", (selectedNums[2]?"#":" "), (selectedNums[3]?"#":" "));
			iprintf(" (4) SF3_A + [%s]    |   (5) SF3_B + [%s]\n", (selectedNums[4]?"#":" "), (selectedNums[5]?"#":" "));
			iprintf(" (6) SF4_A + [%s]    |   (7) SF4_B + [%s]\n", (selectedNums[6]?"#":" "), (selectedNums[7]?"#":" "));
			iprintf(" (8) SF5_A + [%s]    |   (9) SF5_B + [%s]\n", (selectedNums[8]?"#":" "), (selectedNums[9]?"#":" "));
			iprintf(" (A) SF6_A - [%s]    |   (B) SF6_B - [%s]\n", (selectedNums[10]?"#":" "), (selectedNums[11]?"#":" "));
		}
		else{//psu_sni==SnI_TYPE_LIST
			iprintf("\n 	SUPPLY VOLTAGES\n");
			iprintf(" (0) SUP_42V_UNREG [%s]\n", (selectedNums[0]?"#":" "));
			iprintf(" (1) SUP_35V_UNREG [%s]\n", (selectedNums[1]?"#":" "));
			iprintf(" (2) SUP_16V_UNREG [%s]\n", (selectedNums[2]?"#":" "));
			iprintf(" (3) SUP_32V_REG   [%s]\n", (selectedNums[3]?"#":" "));
			iprintf(" (4) SUP_16V_REG   [%s]\n", (selectedNums[4]?"#":" "));
			iprintf(" (5) SUP_12V_F_A   [%s]\n", (selectedNums[5]?"#":" "));
			iprintf(" (6) SUP_12V_F_B   [%s]\n", (selectedNums[6]?"#":" "));
			iprintf(" (7) SUP_12V_F_C   [%s]\n", (selectedNums[7]?"#":" "));
			iprintf(" (8) SUP_n16_REG   [%s]\n", (selectedNums[8]?"#":" "));
			iprintf(" (9) SUP_n20_UNREG [%s]\n", (selectedNums[9]?"#":" "));
			iprintf(" (A) SUP_12V_F_D   [%s]\n", (selectedNums[10]?"#":" "));

			iprintf(" INTERNAL VOLTAGES\n");
			iprintf(" (B) INT_VCC_3V3   [%s]\n", (selectedNums[11]?"#":" "));
			iprintf(" (C) INT_VCC_12V   [%s]\n", (selectedNums[12]?"#":" "));
			iprintf(" (D) INT_VCC_n12V  [%s]\n", (selectedNums[13]?"#":" "));
		}
		selectingNumsAUX[0]=sgetchar(0);
		hexValue = Ascii2Hex(selectingNumsAUX, 1);
		if(((hexValue!=0) || (selectingNumsAUX[0]=='0')) && hexValue<SnI_NUMBER){		selectedNums[hexValue] = !selectedNums[hexValue];} 	// Toggle value if correct
		if (selectingNumsAUX[0]=='p' || selectingNumsAUX[0]=='P'){						memset(selectedNums, 1, sizeof selectedNums);}		// Toggle on all
		if (selectingNumsAUX[0]=='o' || selectingNumsAUX[0]=='O'){						memset(selectedNums, 0, sizeof selectedNums);}		// Toggle off all

	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "%s to be programmed selected  \r\n", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));
}

// Select Alarms
void selectAlarmsINFC (BOOL psu_sni){
	char selectingAlarmsAUX[1] = {'\0'};
	int hexValue=0;
	memset(selectedAlarms, 0, sizeof selectedAlarms);
	while (selectingAlarmsAUX[0]!=' '){
		ERASE_CONSOLE
		iprintf("=============================== Select Alarms =================================\r\n");
		iprintf("Press the keys to toggle selected. # is selected, blank isn't.\n" );
		iprintf("Press SPACE KEY to continue, P to select all, O to unselect all\n" );

		if(categorySelectionINFC=='3' || categorySelectionINFC=='5'){ // Connect/disconnect Alarms
			iprintf("\nSelected(#) Alarms will be CONNECTED\nUnselected( ) will be DISCONNECTED\n");
		}
			iprintf(" (0) Superior Voltage[%s]\n", (selectedAlarms[0]?"#":" "));
			iprintf(" (1) Inferior Voltage[%s]\n", (selectedAlarms[1]?"#":" "));
		if (psu_sni==PSU_TYPE_LIST){
			iprintf(" (2) Superior Current[%s]\n", (selectedAlarms[2]?"#":" "));
			iprintf(" (3) Inferior Current[%s]\n", (selectedAlarms[3]?"#":" "));
		}
		selectingAlarmsAUX[0]=sgetchar(0);
		hexValue = Ascii2Hex(selectingAlarmsAUX, 1);
		if(((hexValue!=0) || (selectingAlarmsAUX[0]=='0')) && (hexValue<ALARM_NUMBER)){	selectedAlarms[hexValue] = !selectedAlarms[hexValue];} 	// Toggle value if correct
		if (selectingAlarmsAUX[0]=='p' || selectingAlarmsAUX[0]=='P'){					memset(selectedAlarms, 1, sizeof selectedAlarms);}		// Toggle on all
		if (selectingAlarmsAUX[0]=='o' || selectingAlarmsAUX[0]=='O'){					memset(selectedAlarms, 0, sizeof selectedAlarms);}		// Toggle off all
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nAlarms selected  \r\n");
}

// Select Alarm Protocols
void selectProtocolsINFC ( BOOL psu_sni ){
	char selectingProtocolsAUX[1] = {'\0'};
	int hexValue=0;
	memset(selectedProtocols, 0, sizeof selectedProtocols);
	while (selectingProtocolsAUX[0]!=' '){
		ERASE_CONSOLE
		iprintf("=========================== Select Alarm Protocols ============================\r\n");
		iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
		iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
		iprintf("Press the keys to toggle selected. # is selected, blank isn't.\n" );
		iprintf("Press SPACE KEY to continue, P to select all, O to unselect all\n" );
		iprintf("\nSelected Protocols are set ON, unselected are set OFF\n");
		iprintf(" (0) Shutdown PSUs [%s]\n", (selectedProtocols[0]?"#":" "));
		iprintf(" (1) Send Message [%s]\n", (selectedProtocols[1]?"#":" "));
		if (psu_sni==PSU_TYPE_LIST){
			iprintf(" (2) Modify Voltage[%s]\n", (selectedProtocols[2]?"#":" "));
		}
		selectingProtocolsAUX[0]=sgetchar(0);
		hexValue = Ascii2Hex(selectingProtocolsAUX, 1);
		if((hexValue!=0 || selectingProtocolsAUX[0]=='0') && hexValue<PROTOCOLS_NUMBER){	selectedProtocols[hexValue] = !selectedProtocols[hexValue];} 	// Toggle value if correct
		if (selectingProtocolsAUX[0]=='p' || selectingProtocolsAUX[0]=='P'){				memset(selectedProtocols, 1, sizeof selectedProtocols);}		// Toggle on all
		if (selectingProtocolsAUX[0]=='o' || selectingProtocolsAUX[0]=='O'){				memset(selectedProtocols, 0, sizeof selectedProtocols);}		// Toggle off all
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nAlarm protocols set  \r\n");
}

// Select PSUs for Protocol Shutdown
void selectShutdownINFC ( BOOL psu_sni ){
	char selectingShutdownAUX[1] = {'\0'};
	int hexValue=0;
	memset(selectedShutdown, 0, sizeof selectedShutdown);
	while (selectingShutdownAUX[0]!=' '){
		ERASE_CONSOLE
		iprintf("============================= Select Shutdown PSUs ============================\r\n");
		iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
		iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
		iprintf("Press the keys to toggle selected. # is selected, blank isn't.\n" );
		iprintf("Press SPACE KEY to continue, P to select all, O to unselect all\n" );
		iprintf("\nSelected (#) PSUs will be SHUTDOWN when executing shutdown protocol\nUnselected ( ) won't be shutdown\n");
		iprintf("\n   PSU OUTPUTS\n");
		iprintf(" (0)SF1_A + [%s]  |  (1)SF1_B + [%s]\n", (selectedShutdown[0]?"#":" "), (selectedShutdown[1]?"#":" "));
		iprintf(" (2)SF2_A + [%s]  |  (3)SF2_B + [%s]\n", (selectedShutdown[2]?"#":" "), (selectedShutdown[3]?"#":" "));
		iprintf(" (4)SF3_A + [%s]  |  (5)SF3_B + [%s]\n", (selectedShutdown[4]?"#":" "), (selectedShutdown[5]?"#":" "));
		iprintf(" (6)SF4_A + [%s]  |  (7)SF4_B + [%s]\n", (selectedShutdown[6]?"#":" "), (selectedShutdown[7]?"#":" "));
		iprintf(" (8)SF5_A + [%s]  |  (9)SF5_B + [%s]\n", (selectedShutdown[8]?"#":" "), (selectedShutdown[9]?"#":" "));
		iprintf(" (A)SF6_A - [%s]  |  (B)SF6_B - [%s]\n", (selectedShutdown[10]?"#":" "), (selectedShutdown[11]?"#":" "));

		selectingShutdownAUX[0]=sgetchar(0);
		hexValue = Ascii2Hex(selectingShutdownAUX, 1);
		if(((hexValue!=0) || (selectingShutdownAUX[0]=='0')) && hexValue<PSU_NUMBER){	selectedShutdown[hexValue] = !selectedShutdown[hexValue];} 	// Toggle value if correct
		if (selectingShutdownAUX[0]=='p' || selectingShutdownAUX[0]=='P'){				memset(selectedShutdown, 1, sizeof selectedShutdown);}		// Toggle on all
		if (selectingShutdownAUX[0]=='o' || selectingShutdownAUX[0]=='O'){				memset(selectedShutdown, 0, sizeof selectedShutdown);}		// Toggle off all
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Shutdown protocol PSUs selected  \r\n");
}



//==================================   PROGRAMMING METHODS   ==========================================//

// (1)
void switchOnOffINFC ( void ){
	char swOnOff;
	iprintf("\n\n");
	iprintf("----------------------------- Switch On/Off PSUs ------------------------------\r\n");
	iprintf("Selected PSUs: "); printBuffer(selectedNums, PSU_NUMBER); iprintf("\n");
	iprintf("Switch ON (1) or OFF (0) the selected PSUs: \n"); swOnOff = sgetchar(0);
	if(swOnOff=='1'){switchONPSUs(selectedNums);	}
	else{			switchOFFPSUs(selectedNums);}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "PSUs switched %s\n", (swOnOff=='1'?"ON":"OFF"));
}

// (2)
void changeOutputVoltageINFC ( void ){
	char psusPosNeg;	char floatScanBuffer[7];						// Scan float value
	float outVolt; int i; BOOL progSuccess = false;
	iprintf("\n\n");
	iprintf("----------------------------- Change voltage PSUs -----------------------------\r\n");
	iprintf("Selected PSUs: "); printBuffer(selectedNums, PSU_NUMBER); iprintf("\n");
	iprintf("Select Positive (1) or Negative (0) PSUs: \n"); psusPosNeg = sgetchar(0);
	if (psusPosNeg=='1'){
		iprintf("Positive PSUs selected\n");
		printf(" Enter the new voltage value (ej.: 10.83) -> ");
			outVolt = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		while (outVolt < 1.26 || outVolt > 32){
			printf(" ERROR: a value between 1.26V and 32V must be given \n");
			printf(" Reenter the new voltage value (ej.: 5.42) -> ");
			outVolt = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		}
		iprintf("Programming...\n");
		for (i=0; i<=SF5_B; i++){if (selectedNums[i]){
			progSuccess = adjustRdac( i, outVolt );
			if(progSuccess){
				psuList[i].progVolt = countsToVolt(outVolt, psuList[i].rShunt);
				iprintf("PSU %d: progValue = %s V (n = %d)\n", i, ftos(psuList[i].progVolt), voltToCounts(outVolt, psuList[i].rShunt));
			}
			else{
				iprintf("PSU %d: voltage programming was unsuccessful\n", i);
			}
		}}
	}
	else{
		iprintf("Negative PSUs selected\n");
		printf(" Enter the new voltage value (ej.: -10.83) -> ");
		outVolt = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		while (outVolt > -1.26 || outVolt < -32){
			printf(" ERROR: a value between -1.26V and -32V must be given \n");
			printf(" Reenter the new voltage value (ej.: -5.42) -> ");
			outVolt = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		}
		iprintf("Programming...\n");
		for (i=SF6_A; i<=SF6_B; i++){if (selectedNums[i]){
			progSuccess = adjustRdac( i, voltToCounts(abs(outVolt), psuList[i].rShunt) ); // TODO: Negative conversion
			if(progSuccess){
				psuList[i].progVolt = (-1)*countsToVolt(abs(outVolt), psuList[i].rShunt);
				iprintf("PSU %d: progValue = %s V (n = %d)\n", i, ftos(psuList[i].progVolt), (-1)*voltToCounts(abs(outVolt), psuList[i].rShunt));
			}
			else{
				iprintf("PSU %d: voltage programming was unsuccessful\n", i);
			}
		}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "PSUs' voltage changed %s\n");
}

// (5)
void programInitializationTimerINFC ( void ){
	char progmInitTimerAUX[5] = {'\0'};
	int initTimer=0, i;
	iprintf("\n\n");
	iprintf("----------------------- Program Initialization Timer --------------------------\r\n");
	iprintf("Selected PSUs: ");printBufferPositions(selectedNums, PSU_NUMBER);	iprintf("\n");
	iprintf(" Enter the time to be programmed (x100ms):  ");
	initTimer = atoi(gets((char *)progmInitTimerAUX));iprintf("\r\n");
	if (initTimer>1000 || initTimer <0){ iprintf(" Invalid Value (%d). Set 0 instead\n", initTimer); initTimer = 0;}
	iprintf(" Programming...\n");
	iprintf("Alarm Limit Times PSUs: \n");
	for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
		psuList[i].initializationTimer = initTimer;
		iprintf("PSU %d: initTimer = %d\n", i, psuList[i].initializationTimer);
	}}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nAlarms programmed with alarmLimitTimes = %d\n", initTimer);

}

// AlmMenu - (1)
void programAlarmLimitTimesINFC ( BOOL psu_sni ){
	char progmAlLimTimeAUX[5] = {'\0'};
	int alLimTime=0, i, j;
	iprintf("\n\n");
	iprintf("-------------------------- Program Alarm Limit Time ---------------------------\r\n");
	iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
	iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
	iprintf(" Enter the time to be programmed (x100ms):  ");
	alLimTime = atoi(gets((char *)progmAlLimTimeAUX));iprintf("\r\n");
	while (alLimTime>1000 || alLimTime <0){
		iprintf(" Invalid Value (%d). Must be [0-1000]\n", alLimTime);
		iprintf(" Reenter the time to be programmed (x100ms):  ");
		alLimTime = atoi(gets((char *)progmAlLimTimeAUX));iprintf("\r\n");
	}
	iprintf(" Programming...\n");
	if(psu_sni==PSU_TYPE_LIST){iprintf("Alarm Limit Times PSUs: \n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				psuList[i].alarmLimitTimes[j] = alLimTime;
			}}
		iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmLimitTimes, ALARM_NUMBER);iprintf("\n");}}
	}
	else{/*psu_sni==SnI_TYPE_LIST*/iprintf("Alarm Limit Times SnIs: \n");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				sniList[i].alarmLimitTimes[j] = alLimTime;
			}}
		iprintf("SnI %d: ", i); printBuffer(sniList[i].alarmLimitTimes, ALARM_NUMBER);iprintf("\n");}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nAlarms programmed with alarmLimitTimes = %d\n", alLimTime);
}

// AlmMenu - (2)
void programAlarmLimitValuesINFC ( BOOL psu_sni ){
	char progmAlLimValAUX[7] = {'\0'};
	float alLimVal=0; int i, j;
	iprintf("\n\n------------------------- Program Alarm Limit Value ---------------------------\r\n");
	iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
	iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
	iprintf(" Enter the Value to be programmed (With 2 decimals top):  ");
	alLimVal = atof(gets((char *)progmAlLimValAUX));iprintf("\r\n");
	while (alLimVal>100 || alLimVal <-100){
		iprintf(" Invalid Value (%d). Must be [-100, 100]\n", alLimVal);
		iprintf(" Reenter the Value to be programmed (With 2 decimals top):  ");
		alLimVal = atof(gets((char *)progmAlLimValAUX));iprintf("\r\n");
	}
	iprintf(" Programming...\n");
	if(psu_sni==PSU_TYPE_LIST){iprintf("Alarm Limit Values PSUs: \n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				psuList[i].alarmLimitValues[j] = alLimVal;
			}}
		iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmLimitValues, ALARM_NUMBER);iprintf("\n");}}
	}
	else{/*psu_sni==SnI_TYPE_LIST*/iprintf("Alarm Limit Values SnIs: \n");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				sniList[i].alarmLimitValues[j] = alLimVal;
			}}
		iprintf("SnI %d: ", i); printBuffer(sniList[i].alarmLimitValues, ALARM_NUMBER);iprintf("\n");}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Alarms programmed with alarmLimitValues = %s\n", ftos(alLimVal));
}

// (3/7)
void programAlarmWatchINFC ( BOOL psu_sni ){
	int i, j;
	iprintf("\n\n---------------------------- Program Alarm Watch ------------------------------\r\n");
	iprintf(" Programming...\n");
	if(psu_sni==PSU_TYPE_LIST){iprintf("Alarm Watch PSUs: \n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){
				psuList[i].alarmWatch[j] = selectedAlarms[j];
			}
		iprintf("PSU %d: [ %s, %s, %s, %s]\n", i, (psuList[i].alarmWatch[0]?"Con":"Dis"), (psuList[i].alarmWatch[1]?"Con":"Dis"), (psuList[i].alarmWatch[2]?"Con":"Dis"), (psuList[i].alarmWatch[3]?"Con":"Dis"));}}
	}
	else{/*psu_sni==SnI_TYPE_LIST*/iprintf("Alarm Watch SnIs: ");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){
				sniList[i].alarmWatch[j] = selectedAlarms[j];
			}
		iprintf("SnI %d: [ %s, %s]\n", i, (sniList[i].alarmWatch[0]?"Con":"Dis"), (sniList[i].alarmWatch[1]?"Con":"Dis"));}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Alarm Watch programmed\n");

}

// AlmMenu - (3)
void programAlarmProtocolsINFC ( BOOL psu_sni ){
	int i, j, k;
	iprintf("\n\n--------------------------- Program Alarm Protocols ---------------------------\r\n");
	iprintf("Selected %s: ", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));printBufferPositions(selectedNums, (psu_sni==PSU_TYPE_LIST?PSU_NUMBER:SnI_NUMBER));	iprintf("\n");
	iprintf("Selected Alarms: "); printBufferPositions(selectedAlarms, ALARM_NUMBER); iprintf("\n");
	iprintf("Selected Alarms: "); printBuffer(selectedProtocols, PROTOCOLS_NUMBER); iprintf("\n");
	iprintf(" Programming...\n");
	if(psu_sni==PSU_TYPE_LIST){iprintf("Alarm Protocols PSUs: \n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){if(selectedAlarms[j]){
				for (k=0; k<PROTOCOLS_NUMBER; k++){
					psuList[i].alarmProtocols[__(j,k)] = selectedProtocols[k];
				}
			}}
		iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmProtocols, PROTOCOLS_NUMBER*ALARM_NUMBER);iprintf("\n");}}
	}
	else{/*psu_sni==SnI_TYPE_LIST*/iprintf("Alarm Protocols SnIs: ");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				for (k=0; k<PROTOCOLS_NUMBER; k++){
					sniList[i].alarmProtocols[__(j,k)] = selectedProtocols[k];
				}
			}}
		iprintf("SnI %d: ", i); printBuffer(sniList[i].alarmProtocols, PROTOCOLS_NUMBER*ALARM_NUMBER);iprintf("\n");}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Alarm Protocols programmed\n");
}

// AlmMenu - (4)
void programAlarmProtocolShutdownINFC(BOOL psu_sni) {
	int i, j; WORD shutdownWord = conversionShutdownARRAYtoWORD(selectedShutdown);
	iprintf("\n\n---------------------- Program Alarm Protocol Shutdown ------------------------\r\n");
	iprintf(" Programming...\n");
	if(psu_sni==PSU_TYPE_LIST){iprintf("Alarm Protocol Shutdown PSUs: \n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){if(selectedAlarms[j]){
				psuList[i].alarmProtocolShutdown[j] = shutdownWord;
			}}
		iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmProtocolShutdown, ALARM_NUMBER);iprintf("\n");}}
	}
	else{/*psu_sni==SnI_TYPE_LIST*/iprintf("Alarm Protocol Shutdown SnIs: ");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){ if(selectedAlarms[j]){
				sniList[i].alarmProtocolShutdown[j] = shutdownWord;
			}}
		iprintf("SnI %d: ", i); printBuffer(sniList[i].alarmProtocolShutdown, ALARM_NUMBER);iprintf("\n");}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Alarm Protocol Shutdown programmed with 0x%x for %s\n", shutdownWord,(psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));
}

// AlmMenu - (5)
void programAlarmProtocolModVoltINFC ( void ) {
	char progModVoltAUX[7] = {'\0'}; char psusPosNeg;
	float progModVolt=0; int i, j;
	iprintf("\n\n------------------- Program Alarm Protocol Modify Voltage ---------------------\r\n");
	iprintf("Selected PSUs: "); printBuffer(selectedNums, PSU_NUMBER); iprintf("\n");
	iprintf("Select Positive (1) or Negative (0) PSUs: \n"); psusPosNeg = sgetchar(0);
	if (psusPosNeg=='1'){
		iprintf("Positive PSUs selected\n");
		iprintf(" Enter the Value to be programmed (10.45):  ");
		progModVolt = atof(gets((char *)progModVoltAUX));iprintf("\r\n");
		while (progModVolt < 1.26 || progModVolt > 32){
			printf(" ERROR: a value between 1.26V and 32V must be given \n");
			printf(" Reenter the new programmed value (ej.: 5.42) -> ");
			progModVolt = atof(gets((char *)progModVoltAUX));iprintf("\r\n");
		}
		iprintf("Programming...\n");
		for (i=0; i<=SF5_B; i++){if (selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){if(selectedAlarms[j]){
					psuList[i].alarmProtocolVoltage[j] = progModVolt;
			}}
			iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmProtocolVoltage, ALARM_NUMBER);iprintf("\n");
		}}
	}
	else /*psusPosNeg=='0'*/{
		iprintf("Negative PSUs selected\n");
		iprintf(" Enter the Value to be programmed (-10.45):  ");
		progModVolt = atof(gets((char *)progModVoltAUX));iprintf("\r\n");
		while (progModVolt < -32 || progModVolt > -1.26){
			printf(" ERROR: a value between -32V and -1.26V must be given \n");
			printf(" Reenter the new programmed value (ej.: -5.42) -> ");
			progModVolt = atof(gets((char *)progModVoltAUX));iprintf("\r\n");
		}
		iprintf("Programming...\n");
		for (i=0; i<=SF5_B; i++){if (selectedNums[i]){
			for (j=0; j<ALARM_NUMBER; j++){if(selectedAlarms[j]){
					psuList[i].alarmProtocolVoltage[j] = progModVolt;
			}}
			iprintf("PSU %d: ", i); printBuffer(psuList[i].alarmProtocolVoltage, ALARM_NUMBER);iprintf("\n");
		}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "Alarm Protocol modify Voltage programmed with %s\n", ftos(progModVolt));
}

// (+/-)
void programDefaultValuesINFC ( BOOL psu_sni ){
	int i, lastNum = 0;
	iprintf("\n\n------------------- Program Default Values %s ---------------------\r\n", (psu_sni==PSU_TYPE_LIST?"PSUs":"SnIs"));

	if(psu_sni==PSU_TYPE_LIST){
		iprintf("Selected PSUs: "); printBuffer(selectedNums, PSU_NUMBER); iprintf("\n");
		iprintf("Default Values PSUs: ");
		iprintf("Programming...\n");
		for (i=0; i<PSU_NUMBER; i++){if(selectedNums[i]){	lastNum = i;
			defaultValuesPSU(i, false);
		}}
		printValuesPSU(lastNum);
		iprintf("\nUpdating Voltages\n");
		updateVoltagePSUs(selectedNums);
	}
	else{/*psu_sni==SnI_TYPE_LIST*/
		iprintf("Selected SnIs: "); printBuffer(selectedNums, SnI_NUMBER); iprintf("\n");
		iprintf("Default Values SnIs: ");
		iprintf("Programming...\n");
		for (i=0; i<SnI_NUMBER; i++){if(selectedNums[i]){
			defaultValuesSnI(i);
		}}
	}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("Default Values programmed\n");

}

