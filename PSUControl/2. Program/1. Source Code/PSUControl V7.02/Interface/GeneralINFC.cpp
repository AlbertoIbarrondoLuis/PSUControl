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
// Used for Keyboard
static char c;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD c = sgetchar(0);
char globalSelectionINFC='0';
char categorySelectionINFC='0';

// Pause Tasks
BOOL idleMode_Alarm = CONFIG_INIT_PAUSE_TASKS;
BOOL seqMode_Monitor = CONFIG_INIT_PAUSE_TASKS;

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
				case '1': DisplayINFC(); break;
				case '2': ProgramINFC(); break;
				case '3': LibrariesINFC(); break;
				case '4': TestMain(); break;
				case '5': ConfigINFC(); break;
				case '+':
					idleMode_Alarm = !idleMode_Alarm;
					iddleMode_AlarmCTRL_Task (idleMode_Alarm);
					iprintf("AlarmTask set to %sMode\n", (idleMode_Alarm?"idle":"normal"));
					categorySelectionINFC=EXIT;WAIT_FOR_KEYBOARD
					break;
				case '-':
					seqMode_Monitor = !seqMode_Monitor;
					sequentialMode_MonitorCTRL_Task(seqMode_Monitor);
					iprintf("monitorTask set to %sMode\n", (seqMode_Monitor?"sequential":"normal"));
					categorySelectionINFC=EXIT;WAIT_FOR_KEYBOARD
					break;
				case '*':
					if (seqMode_Monitor){
						sequentialMode_MonitorCTRL_Task(true);
						iprintf("monitorTask advanced once in sequentialMode\n");
					}
					categorySelectionINFC=EXIT;WAIT_FOR_KEYBOARD
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
	iprintf(" (1) Display Menu\r\n" );
	iprintf(" (2) Program Menu\r\n" );
	iprintf(" (3) Libraries Menu\r\n" );
	iprintf(" (4) Test Menu\r\n" );
	iprintf(" (5) Configuration Menu\r\n" );
	iprintf("\n (+) Toggle Alarm Task mode (now %s)\r\n", (idleMode_Alarm?"idleMode":"normalMode"));
	iprintf(" (-) Toggle Monitor Task mode (now %s)\r\n", (seqMode_Monitor?"sequentialMode":"normalMode"));
	if(seqMode_Monitor){iprintf(" (*) One time Monitor Task (only in sequentialMode)\r\n");}
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nSelect menu number: " );
	globalSelectionINFC = sgetchar( 0 ); iprintf("%c\n", globalSelectionINFC);
	iprintf("-------------------------------------------------------------------------------\r\n" );
}



