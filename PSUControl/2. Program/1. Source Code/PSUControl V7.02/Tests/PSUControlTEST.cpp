/*
 * PSUControlTEST.cpp
 *
 *	Tests for the whole system
 *
 *  Created on: 23-jun-2015
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

//---------Imported from generalTEST.cpp
// Results
extern BYTE result1; extern BYTE result2; extern BYTE result3; extern BYTE result4; extern BYTE result5;
extern BYTE result6; extern BYTE result7; extern BYTE result8; extern BYTE result9; extern BYTE result10;
extern BYTE resultTotal;

//=====================================================================================================//
//================================    PSUControl Testing METHODS    ===================================//
//=====================================================================================================//

//-------------------------------------------   MENU    -----------------------------------------------//

void PSUControlTEST ( void ) {
	ERASE_CONSOLE
	iprintf("============================== PSUControl TEST MENU ===========================\r\n" );
	iprintf( "\n (e) EXIT TO GENERAL MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf( "\r\nEnter command: " );
	categorySelectionTEST = sgetchar( 0 ); iprintf ("%c\n", categorySelectionTEST);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
	switch ( categorySelectionTEST ){
		case '1': 	break;
		case EXIT: 	iprintf( "\nGOING BACK TO test MENU\n");	categorySelectionTEST = EXIT;	break;
		default:	iprintf( "\nINVALID COMMAND -> %c\r\n", categorySelectionTEST);				break;
	}
	if (categorySelectionTEST!=EXIT){WAIT_FOR_KEYBOARD}
}

// TODO: Define global tests for the whole system
