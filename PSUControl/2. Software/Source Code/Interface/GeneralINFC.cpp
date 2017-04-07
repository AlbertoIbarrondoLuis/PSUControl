/*
 * GeneralINFC.cpp
 *
 *	Interface menu (top level menu). Call to access all the other menus via console. Can be exited
 *
 *  Created on: 21/05/2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface.h"


//==============================================VARIABLES==============================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];
extern SnI_TYPE supList[SUP_NUMBER];

// Used for Keyboard
static char c;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD c = sgetchar(0);
char globalSelectionINFC='0';
char categorySelectionINFC='0';
extern BYTE i2CAddressArray[PSU_NUMBER];

// Pause Tasks
BOOL idleMode_Alarm = CONFIG_INIT_PAUSE_TASKS;
BOOL seqMode_Monitor = CONFIG_INIT_PAUSE_TASKS;

// Print values in MonitorTask
extern BOOL config_MonitorCTRL_PrintVals;

// Radar Voltages
float radarVolts[12] = RADAR_INIT_VOLTS;
int radarCounts[12] = RADAR_INIT_COUNTS;
//=====================================================================================================//
//==================================== PUBLIC INTERFACE MENU ==========================================//
//=====================================================================================================//

void InferfaceMain ( void ){
	globalSelectionINFC='0';
	while (globalSelectionINFC!=EXIT){
		InterfaceMenuINFC();
		categorySelectionINFC = '0';
		while (categorySelectionINFC!=EXIT){
			switch (globalSelectionINFC){
				case '0': SetRadarVoltages(); break;
				case '1': AdjRadarVoltages(); break;
				case '2': SwitchPSUs(); break;
				case '3': ChangeVoltPSUs(); break;
				//case '3': ProgramINFC(); break;
				case '4': DisplayINFC(); break;
				case '5': LibrariesINFC(); break;
//				case '6': TestMain(); break;		// Disabled for the moment. Block included in project PSUNonUsedBlocks
				case '7': ConfigINFC(); break;
				case '+':
					idleMode_Alarm = !idleMode_Alarm;
					iddleMode_AlarmCTRL_Task (idleMode_Alarm);
					iprintf("AlarmTask set to %sMode\n", (idleMode_Alarm?"idle":"normal"));
					categorySelectionINFC=EXIT;
					break;
				case '-':
					seqMode_Monitor = !seqMode_Monitor;
					sequentialMode_MonitorCTRL_Task(seqMode_Monitor);
					iprintf("monitorTask set to %sMode\n", (seqMode_Monitor?"sequential":"normal"));
					categorySelectionINFC=EXIT;
					break;
				case '*':
					if (seqMode_Monitor){
						sequentialMode_MonitorCTRL_Task(true);
						iprintf("monitorTask advanced once in sequentialMode\n");
					}
					categorySelectionINFC=EXIT;WAIT_FOR_KEYBOARD
					break;
				case '.':
					toggle_MonitorCTRL_PrintVals(!config_MonitorCTRL_PrintVals);
					iprintf("monitorTask toggled to %s print Values\n", (config_MonitorCTRL_PrintVals?"":"NOT"));
					categorySelectionINFC=EXIT;
					break;
				default:
					iprintf( "\nINVALID KEY -> %c\r\n", globalSelectionINFC);
					iprintf( " \r\nPRESS ANY KEY TO RETURN TO GENERAL MENU\r\n" );
					WAIT_FOR_KEYBOARD
					categorySelectionINFC=EXIT;
					break;
			}
			iprintf("-------------------------------------------------------------------------------\r\n" );
		}
	}
}

void InterfaceMenuINFC ( void ){
	ERASE_CONSOLE
	iprintf("===============================================================================\r\n" );
	iprintf("=============================== INTERFACE MENU ================================\r\n" );
	iprintf("===============================================================================\r\n" );
	iprintf(" (0) Set Radar Voltages\r\n" );
	iprintf(" (1) Fine Adjust of Voltages\r\n" );
	iprintf(" (2) Switch On/Off PSUs\r\n" );
	iprintf(" (3) Change output Voltages\r\n\n" );
	iprintf(" (-) Toggle Monitoring (now %s)\r\n", (seqMode_Monitor?"sequentialMode":"normalMode"));


	iprintf("\nDEVELOPER MENUS\n" );
	iprintf(" (4) Display Menu\r\n" );
	iprintf(" (5) Libraries Menu\r\n" );
	iprintf(" (6) Test Menu\r\n" );
	iprintf(" (7) Configuration Menu\r\n" );
	iprintf("\n (+) Toggle Alarm Task mode (now %s)\r\n", (idleMode_Alarm?"idleMode":"normalMode"));
	iprintf(" (-) Toggle Monitor Task mode (now %s)\r\n", (seqMode_Monitor?"sequentialMode":"normalMode"));
	iprintf(" (.) Toggle value Print in MonitorTask (now %s)\r\n", (config_MonitorCTRL_PrintVals?"ON":"OFF"));
	if(seqMode_Monitor){iprintf(" (*) One time Monitor Task (only in sequentialMode)\r\n");}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nSelect menu number: " );
	globalSelectionINFC = sgetchar( 0 ); iprintf("%c\n", globalSelectionINFC);
	iprintf("-------------------------------------------------------------------------------\r\n" );
}

// (0)
void SetRadarVoltages ( void ){
	int i = 0; BOOL res = false;
	iprintf(" 0. Setting all PSUs to the RADAR voltages\n");

		for (i = 0; i<PSU_NUMBER; i++){	// Adjusting Voltage
			iprintf("        .Setting SF%d_%s to Voltage  %s -> ", (i&0x1?(i-1)/2+1:i/2+1), (i&0x1?"B":"A"), ftos(radarVolts[i],1));
			res = (adjustRdac (i, radarCounts[i], 1)!=0);
			iprintf(" %s\n", (res?"OK":"ERROR"));
			OSTimeDly(TICKS_PER_SECOND/10);
		}
		iprintf("        .Waiting for voltages to adjust\n");
		OSTimeDly(TICKS_PER_SECOND*2);
		for (i = 0; i<PSU_NUMBER; i++){
			// Connecting Relay
			iprintf("        .Connecting SF%d_%s Relay   -> ", (i&0x1?(i-1)/2+1:i/2+1), (i&0x1?"B":"A") );
			connectRelay(i);
			OSTimeDly(TICKS_PER_SECOND/2);
		}
		iprintf("    - EXITING TO MAIN MENU\n");
		categorySelectionINFC=EXIT;
}


// (1)
void AdjRadarVoltages ( void ){
	char cardSelectionINFC='\0';
	char psuSelectionINFC='\0';
	int rdacVal; BYTE psuNum;
	iprintf(" 1. Manual adjustment of voltages\n");
		// Number
		iprintf("    - SELECT A REGULATION SLOTCARD (1 to 6): ");
		cardSelectionINFC = sgetchar( 0 )-0x30; iprintf("%d \n", cardSelectionINFC);
		if (cardSelectionINFC>6 || cardSelectionINFC<1){
			iprintf("       Invalid number (%d)\n", cardSelectionINFC);	return;}
		// Letter
		iprintf("    - SELECT THE PSU (A or B): ");
		psuSelectionINFC = sgetchar( 0 ); iprintf("%c \n", psuSelectionINFC);
		if (psuSelectionINFC!='a' && psuSelectionINFC!='A' && psuSelectionINFC!='B' && psuSelectionINFC!='b'){
			iprintf("       Invalid PSU (%c)\n", psuSelectionINFC);	return;}

	iprintf("\n Entering Adjustment mode for SF%d_%c. Press 'e' to EXIT.\n", cardSelectionINFC, psuSelectionINFC);
	psuNum = 2*(cardSelectionINFC-1) + (psuSelectionINFC=='a' || psuSelectionINFC=='A'?0:1);
	rdacVal = getValRDAC(psuList[psuNum].rdacAdr, psuList[psuNum].bridgeI2CAdr);
	while (cardSelectionINFC!=EXIT){
		iprintf("  Update Volt [RDAC: %d].  (Press '+' or '-') => ", rdacVal);
		cardSelectionINFC = sgetchar( 0 ); iprintf("%c \n", cardSelectionINFC);
		rdacVal += (cardSelectionINFC=='-'?1:(cardSelectionINFC=='+'?-1:0));
		setValRDAC(rdacVal, psuList[psuNum].rdacAdr, psuList[psuNum].bridgeI2CAdr);
		rdacVal = getValRDAC(psuList[psuNum].rdacAdr, psuList[psuNum].bridgeI2CAdr);
		psuList[psuNum].progCounts = rdacVal;
	}
	iprintf("    - EXITING TO MAIN MENU\n");
	categorySelectionINFC=EXIT;

}

//(2)
void SwitchPSUs ( void ){
	iprintf(" 2. Switch On/Off PSUs\n");
	selectNumsINFC(PSU_TYPE_LIST);
	switchOnOffINFC();
	iprintf("    - EXITING TO MAIN MENU\n");WAIT_FOR_KEYBOARD
	categorySelectionINFC=EXIT;
}

// (3)
void ChangeVoltPSUs (void){
	char cardSelectionINFC='\0';
	char psuSelectionINFC='\0';
	float outVolt; BYTE psuNum; BOOL isNegative;
	iprintf(" 3. Setting output voltage for a PSU\n");
		// Number
		iprintf("    - SELECT A REGULATION SLOTCARD (1 to 6): ");
		cardSelectionINFC = sgetchar( 0 )-0x30; iprintf("%d \n", cardSelectionINFC);
		if (cardSelectionINFC>6 || cardSelectionINFC<1){
			iprintf("       Invalid number (%d)\n", cardSelectionINFC);	return;}
		// Letter
		iprintf("    - SELECT THE PSU (A or B): ");
		psuSelectionINFC = sgetchar( 0 ); iprintf("%c \n", psuSelectionINFC);
		if (psuSelectionINFC!='a' && psuSelectionINFC!='A' && psuSelectionINFC!='B' && psuSelectionINFC!='b'){
			iprintf("       Invalid PSU (%c)\n", psuSelectionINFC);	return;}

	psuNum = 2*(cardSelectionINFC-1) + (psuSelectionINFC=='a' || psuSelectionINFC=='A'?0:1);
	isNegative = psuNum>SF5_B;

	if (isNegative)	{	outVolt = scanNegativeFloatValue();	}
	else			{	outVolt = scanPositiveFloatValue();	}

	if(adjustRdac( psuNum, outVolt )){	iprintf("PSU %d: progValue = %s V (n = %d)\n", \
			psuNum, ftos(psuList[psuNum].progVolt), voltPositiveToCounts(outVolt, psuList[psuNum].rShunt));}
	else{								iprintf("PSU %d: voltage programming was unsuccessful\n", psuNum); }

	iprintf("    - EXITING TO MAIN MENU\n");
	categorySelectionINFC=EXIT;
}
