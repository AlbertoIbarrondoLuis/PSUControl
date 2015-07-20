/*
 * LibrariesINFC.cpp
 *
 *	Interface access to all the libraries' functions contained in Interface/LibrariesInterface
 *
 *  Created on: 23/06/2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface.h"
#include "Libraries/Libraries.h"


//==============================================VARIABLES==============================================//
// Keyboard
extern char categorySelectionINFC;
char functionSelectionINFC ='\0';
char auxSelectionINFC = '\0';
char funcChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD funcChar = sgetchar(0);

// I2C&SPI, RDAC
#define CHANGE_SLAVE slaveSPIAddressINFC=(psuNumINFC&0x1?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS);
BYTE bufferINFC[I2C_MAX_BUF_SIZE];
BYTE slaveSPIAddressINFC = LOWER_SLAVE_SPI_ADDRESS;	// Set to UPPER_SLAVE_SPI_ADDRESS or LOWER_SLAVE_SPI_ADDRESS
BYTE i2cAddressINFC = 0x2F;								// Defined by 3 switches for RDACs. 0x28 to 0x2F

// Values
float floatValue=0;
int psuNumINFC;
int newvalueINFC;
BOOL ctrl_allowINFC = false;

// Pause Tasks
extern BOOL idleMode_Alarm;
extern BOOL seqMode_Monitor;


//=====================================================================================================//
//===================================== PUBLIC LIBRARIES MENU =========================================//
//=====================================================================================================//

void LibrariesINFC ( void ){
	// Setting alarm and monitor Tasks into Pause status
	if (!idleMode_Alarm)	{ iddleMode_AlarmCTRL_Task (true);		}
	if (!seqMode_Monitor)	{ sequentialMode_MonitorCTRL_Task(true);}

	ERASE_CONSOLE
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("------------------------------- LIBRARIES MENU --------------------------------\r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf(" (1) I2C&SPILibrary - Communications\n");
	iprintf(" (2) RDACLibrary - Digital Rheostats\n");
	iprintf(" (3) AGCLibrary - Automatic Gain Control\n");
	iprintf(" (4) RelayLibrary - Relays\n");
	iprintf(" (5) MUXLibrary - Voltage and Current measuring\n");
	iprintf("\n (e) EXIT TO GENERAL MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nEnter command:  " );
	categorySelectionINFC = sgetchar( 0 ); iprintf("%c\n", categorySelectionINFC);
	iprintf("\n-------------------------------------------------------------------------------\r\n" );
	processCommandLibraryINFC();
}



//=====================================================================================================//
//=================================== PRIVATE LIBRARIES METHODS =======================================//
//=====================================================================================================//

// Choose Library
void processCommandLibraryINFC ( void ){
	switch (categorySelectionINFC){
		case '1': I2CnSPILibraryINFC(); break;
		case '2': RDACLibraryINFC(); break;
		case '3': AGCLibraryINFC(); break;
		case '4': RelayFunctionsDISP(); break;
		case '5': MUXLibraryINFC(); break;
		case EXIT:
			iprintf("Exiting to INTERFACE MENU\n");
			// Setting alarm and monitor Tasks into previous status
			iddleMode_AlarmCTRL_Task (idleMode_Alarm);
			sequentialMode_MonitorCTRL_Task(seqMode_Monitor);
			break;
		default:
				iprintf( "\nINVALID KEY -> %c\r\n", categorySelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO CONFIG MENU\r\n" );
				break;
	}
	WAIT_FOR_KEYBOARD
}

