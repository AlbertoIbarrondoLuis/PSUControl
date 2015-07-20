/*
 * RelayLibraryINFC.cpp
 *
 *	Interface Menu and Methods for RelayLibrary
 *
 *  Created on: 23-jun-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods

//==============================================VARIABLES==============================================//
// Keyboard
#define WAIT_FOR_KEYBOARD funcChar = sgetchar(0);
extern char funcChar;				// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
extern char functionSelectionINFC;

// Values
extern int psuNumINFC;


//=====================================================================================================//
//============================= PUBLIC RELAY LIBRARY INTERFACE MENU ===================================//
//=====================================================================================================//

void RelayFunctionsDISP ( void ) {
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("=============================== RELAY LIBRARY =================================\r\n" );
		iprintf(" (1) Connect PSU Relay\n");
		iprintf(" (2) Disconnect PSU Relay\n");
		iprintf(" (3) Relays' Status\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '1':	connectRelay_Relay_INFC();		break;
			case '2':	disconnectRelay_Relay_INFC();	break;
			case '3':	relayStatus_Relay_INFC();		break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO RELAY LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}




//=====================================================================================================//
//============================ PRIVATE RELAY LIBRARY INTERFACE METHODS ================================//
//=====================================================================================================//

// (1)
void connectRelay_Relay_INFC ( void ) {
	iprintf("\n\n\n\n(1). Connect Relay\n");
	psuNumINFC = selectPSUNumINFC();
	connectPSU( psuNumINFC );
	iprintf("Relay connected for PSU %d\n", psuNumINFC);
}

// (2)
void disconnectRelay_Relay_INFC ( void ) {
	iprintf("\n\n\n\n(2). Disconnect Relay\n");
	psuNumINFC = selectPSUNumINFC();
	disconnectPSU( psuNumINFC );
	iprintf("Relay disconnected for PSU %d\n", psuNumINFC);
}

// (3)
void relayStatus_Relay_INFC ( void ){
	iprintf("\n\n\n\n(3). Relay Status\n");
	for (int i=0; i<PSU_NUMBER; i++){
		iprintf("%d: %s\n", i, (getStatusRelay(i)?"CONNECTED":"DISCONNECTED"));
	}
}
