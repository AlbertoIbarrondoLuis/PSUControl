/*
 * MUXLibrary.cpp
 *
 *	Interface Menu and methods for MUXLibrary
 *
 *  Created on: 23-jun-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods

//==============================================VARIABLES==============================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];			// MAIN PSU ARRAY LIST
extern SnI_TYPE sniList[INT_VCC_n12V + 1];		// Supply & Internal voltages List

// Keyboard
#define WAIT_FOR_KEYBOARD funcChar = sgetchar(0);
extern char funcChar;				// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
extern char functionSelectionINFC;
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];


//=====================================================================================================//
//=============================== PUBLIC MUX LIBRARY INTERFACE MENU ===================================//
//=====================================================================================================//

void MUXLibraryINFC ( void ){
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("================================ MUX LIBRARY ==================================\r\n" );
		iprintf(" (1) Set Muxes for a reading\n");
		iprintf(" (2) Read PSU voltage\n");
		iprintf(" (3) Read PSU current\n");
		iprintf(" (4) Read SnI voltage\n");
		iprintf(" (5) Get Muxes' status\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '1': setMuxes_MUX_INFC();			break;
			case '2': readPSUVoltage_MUX_INFC();	break;
			case '3': readPSUCurrent_MUX_INFC();	break;
			case '4': readSnIVoltage_MUX_INFC();	break;
			case '5': getMuxesStatus_MUX_INFC();	break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO MUX LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}




//=====================================================================================================//
//============================== PRIVATE MUX LIBRARY INTERFACE METHODS ================================//
//=====================================================================================================//

// (1)
void setMuxes_MUX_INFC ( void ) {
	int selectSamplFunc; int Num;
	iprintf("\n\n\n\n(1). Set Muxes for a reading\n");
	iprintf("Select Sampling Function: \n");
	iprintf(" (0) FUNCTION_PSU_VOLTAGE\n");
	iprintf(" (1) FUNCTION_PSU_CURRENT\n");
	iprintf(" (2) FUNCTION_SnI_VOLTAGE\n");
	selectSamplFunc = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while (selectSamplFunc<0 || selectSamplFunc>2){
		iprintf ("Invalid value. Reselect sampling function: ");
		selectSamplFunc = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	iprintf("Select PSU/SnI number: ");
	*bufferINFC = 0x00000000;
	Num=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while (Num<0 || (Num>PSU_NUMBER && selectSamplFunc!=FUNCTION_SnI_VOLTAGE) || (Num>SnI_NUMBER && selectSamplFunc==FUNCTION_SnI_VOLTAGE)){
		iprintf ("Invalid value. Reselect PSU/SnI number: ");
		Num = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	setMUX(selectSamplFunc, Num);
	iprintf("Muxes set for sampling function %d and psu/sni %d\n", selectSamplFunc, Num);
}

// (2)
void readPSUVoltage_MUX_INFC ( void ){
	int psuNum;
	iprintf("\n\n\n\n(2). Read PSU voltage\n");
	iprintf("Select PSU number: ");
	psuNum=selectPSUNumINFC();
	iprintf("Reading Voltage...");
	readVoltageValue(psuNum, PSU_TYPE_LIST);
	iprintf("Voltage for PSU %d: %s\n", psuNum, ftos(psuList[psuNum].vOut));
}

// (3)
void readPSUCurrent_MUX_INFC ( void ){
	int psuNum;
	iprintf("\n\n\n\n(3). Read PSU Current\n");
	iprintf("Select PSU number: ");
	psuNum=selectPSUNumINFC();
	iprintf("Reading Current...");
	readCurrentValue(psuNum);
	iprintf("Current for PSU %d: %s\n", psuNum, ftos(psuList[psuNum].cOut));
}

// (4)
void readSnIVoltage_MUX_INFC ( void ){
	int sniNum;
	iprintf("\n\n\n\n(4). Read SnI voltage\n");
	iprintf("Select SnI number: ");
	*bufferINFC = 0x00000000;
	sniNum=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while (sniNum<0 || sniNum>=SnI_NUMBER){
		iprintf ("Invalid value. Reselect PSU/SnI number: ");
		sniNum = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	iprintf("Reading Voltage...");
	readVoltageValue(sniNum, SnI_TYPE_LIST);
	iprintf("Voltage for SnI %d: %s\n", sniNum, ftos(sniList[sniNum].vOut));
}

// (5)
void getMuxesStatus_MUX_INFC ( void ){
	iprintf("\n\n\n\n(5). Get Muxes' status\n");
	iprintf("Sampling Function: %d\n", getSamplingFunctionMUX());
	iprintf("Number: %d\n", getNumMUX());

}
