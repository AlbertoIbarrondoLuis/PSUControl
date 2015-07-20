/*
 * MainTEST.cpp
 *
 *	Manages a console access to the different test modules (Controller, Libraries, PSUcontrol).
 *	Use TestMain().
 *
 *  Created on: 10-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Tests/Tests.h"


//==============================================VARIABLES==============================================//
// Keyboard
char testChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD testChar = sgetchar(0);
char globalSelectionTEST;
char categorySelectionTEST;
extern char categorySelectionINFC;

// Imported Arrays
//extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
//extern SnI_TYPE sniList [INT_VCC_n12V + 1];			// Supply & Internal voltages List

// Auxiliary
int newvalue;

// Results
BYTE results[15];										// Stores partial test results
BYTE resultTotal;										// Stores global test result



//=====================================================================================================//
//=====================================    MAIN TEST METHODS    =======================================//
//=====================================================================================================//

void TestMain ( void ){
	globalSelectionTEST = '0';
	while (globalSelectionTEST!=EXIT){
		TestMenu();
		categorySelectionTEST = '0';
		while (categorySelectionTEST!=EXIT){
			switch (globalSelectionTEST){
				case '1':	LibrariesTEST();	break;
				case '2': 	ControllerTEST();	break;
				case '3': 	PSUControlTEST();	break;
				case EXIT: 	iprintf( "\nGOING BACK TO INTERFACE MENU\n"); categorySelectionINFC = EXIT; break;
				default:
					iprintf( "\nINVALID MENU NUMBER -> %c\r\n", categorySelectionTEST);
					iprintf( " \r\nPRESS ANY KEY TO RETURN TO TEST MENU\r\n" );
					categorySelectionTEST = EXIT;
				break;
			}
			WAIT_FOR_KEYBOARD
		}
	}
}


void TestMenu ( void ){
	ERASE_CONSOLE
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("--------------------------------- TEST MENU -----------------------------------\r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf(" (1) Libraries' Tests\r\n" );
	iprintf(" (2) Controller Tests\r\n" );
	iprintf(" (2) PSUControl Tests\r\n" );
	iprintf("\n (e) EXIT TO INTERFACE MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nSelect menu number: " );
	globalSelectionTEST = sgetchar( 0 ); iprintf("%c\n", globalSelectionTEST);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
}
