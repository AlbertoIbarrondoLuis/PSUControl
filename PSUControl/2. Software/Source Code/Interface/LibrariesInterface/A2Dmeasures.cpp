/*
 * A2Dmeasures.cpp
 *
 *  Created on: 24/09/2015
 *      Author: Alberto
 */



#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods

//==============================================VARIABLES==============================================//

// Keyboard
#define WAIT_FOR_KEYBOARD funcChar = sgetchar(0);
extern char funcChar;				// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
extern char functionSelectionINFC;


//=====================================================================================================//
//=============================== PUBLIC MUX LIBRARY INTERFACE MENU ===================================//
//=====================================================================================================//

void A2DmeasuresINFC ( void ){
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("================================ A2D MEASURING ================================\r\n" );
		iprintf( "MUX/AGC SETTING FUNCTIONS\n" );
		iprintf(" (1) Set Muxes for a reading\n");
		iprintf(" (2) Set AGC Gain\n");
		iprintf( "READING FUNCTIONS\n" );
		iprintf(" (4) Read A2D\n");
		iprintf(" (5) Get Muxes' status\n");
		iprintf(" (6) Get AGC's Current Gain and Counts\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '1': setMuxes_MUX_INFC();			break;
			case '2': setAGCGain_AGC_INFC();		break;
			case '4': readA2D_MUX_INFC();			break;
			case '5': getMuxesStatus_MUX_INFC();	break;
			case '6':	iprintf("\n\n\n\n(6). Get AGC values\n"); getAGCStatus_AGC_INFC();		break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO MUX LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}
