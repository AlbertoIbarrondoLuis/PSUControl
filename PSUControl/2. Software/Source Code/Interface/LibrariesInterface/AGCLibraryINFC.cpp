/*
 * AGCLibraryINFC.cpp
 *
 *	Interface menu and methods for AGCLibrary
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
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];

// AGC
extern BOOL maxAGCReached;
extern BOOL minAGCReached;


//=====================================================================================================//
//============================== PUBLIC AGC LIBRARY INTERFACE MENU ====================================//
//=====================================================================================================//

void AGCLibraryINFC ( void ){
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("================================= AGC LIBRARY =================================\r\n" );
		iprintf(" (1) Minimum AGC Gain\n");
		iprintf(" (2) Maximum AGC Gain\n");
		iprintf(" (3) Scale AGC Gain\n");
		iprintf(" (4) Set AGC Gain\n");
		iprintf(" (5) Convert Gain to RDAC Counts\n");
		iprintf(" (6) Convert RDAC Counts to Gain \n");
		iprintf(" (7) Get Current Gain and Counts\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '1':	setAGCMinimum_AGC_INFC();		break;
			case '2':	setAGCMaximum_AGC_INFC();		break;
			case '3':	scaleAGCGain_AGC_INFC();		break;
			case '4':	setAGCGain_AGC_INFC();			break;
			case '5':	countsToGain_AGC_INFC();		break;
			case '6':	gainToCounts_AGC_INFC();		break;
			case '7':	iprintf("\n\n\n\n(7). Get values\n"); getAGCStatus_AGC_INFC();		break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO AGC LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}



//=====================================================================================================//
//============================ PRIVATE AGC LIBRARY INTERFACE METHODS ==================================//
//=====================================================================================================//

// (1)
void setAGCMinimum_AGC_INFC ( void ){
	iprintf("\n\n\n\n(1). Setting AGC gain to Minimum\n");
	minAGC();
	getAGCStatus_AGC_INFC();
}

// (2)
void setAGCMaximum_AGC_INFC ( void ){
	iprintf("\n\n\n\n(2). Setting AGC gain to maximum\n");
	maxAGC();
	getAGCStatus_AGC_INFC();
}

// (3)
void scaleAGCGain_AGC_INFC ( void ){
	float newScaleFactor;
	iprintf("\n\n\n\n(3). Scaling AGC gain\n");
	iprintf("Current AGC gain: %s\n", ftos(getGainAGC(), 2));
	iprintf("Scale factor: \n");
	*bufferINFC = 0x00000000;
	newScaleFactor=atof(gets((char*)bufferINFC));iprintf("\r\n");
	scaleGainAGC(newScaleFactor);
	getAGCStatus_AGC_INFC();
}

// (4)
void setAGCGain_AGC_INFC ( void ){
	float newGain;
	iprintf("\n\n\n\n(4). Set AGC gain\n");
	iprintf("Current AGC gain: %s\n", ftos(getGainAGC(),2));
	iprintf("New Value: \n");
	*bufferINFC = 0x00000000;
	newGain=atof(gets((char*)bufferINFC));iprintf("\r\n");
	setGainAGC(newGain);
	getAGCStatus_AGC_INFC();
}

// (5)
void countsToGain_AGC_INFC ( void ) {
	float counts;
	iprintf("\n\n\n\n(5). Conversion: Counts to Gain\n");
	iprintf("Counts: ");
	*bufferINFC = 0x00000000;
	counts=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf("Converted gain: %d\n", countsToGainAGC(counts));
}

// (6)
void gainToCounts_AGC_INFC ( void ) {
	float newGain;
	iprintf("\n\n\n\n(6). Conversion: Gain to Counts\n");
	iprintf("Gain: ");
	newGain=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf("Converted Counts: %d\n", gainToCountsAGC(newGain));
}

// (7)
void getAGCStatus_AGC_INFC ( void ) {
	iprintf("AGC RAM Gain: %s (C:0%x)\n", ftos(getGainAGC(), 4), getCountsAGC());
	readValuesAGC();
	iprintf("AGC Read Gain: %s (C:0%x)\n", ftos(getGainAGC(), 4), getCountsAGC());
	iprintf("I2C Result: 0x%x ( 0 is I2C_OK)\n", getI2CResultAGC());
	iprintf("AGC minimum reached: %s\n",(minAGCReached?"YES":"NO"));
	iprintf("AGC maximum reached: %s\n",(maxAGCReached?"YES":"NO"));
}
