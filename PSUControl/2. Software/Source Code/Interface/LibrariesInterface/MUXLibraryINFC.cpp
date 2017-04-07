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
extern SnI_TYPE supList[SUP_NUMBER];			// Supply voltages List

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
		iprintf(" (0) Toggle Individual Pins\n");
		iprintf(" (1) Set Muxes for a reading\n");
		iprintf(" (2) Read PSU voltage\n");
		iprintf(" (3) Read PSU current\n");
		iprintf(" (4) Read SnI voltage\n");
		iprintf(" (5) Read a2d\n");
		iprintf(" (6) Get Muxes' status\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '0': setMuxesPins_MUX_INFC();		break;
			case '1': setMuxes_MUX_INFC();			break;
			case '2': readPSUVoltage_MUX_INFC();	break;
			case '3': readPSUCurrent_MUX_INFC();	break;
			case '4': readSnIVoltage_MUX_INFC();	break;
			case '5': readA2D_MUX_INFC();			break;
			case '6': getMuxesStatus_MUX_INFC();	break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( "\r\nPRESS ANY KEY TO RETURN TO MUX LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}




//=====================================================================================================//
//============================== PRIVATE MUX LIBRARY INTERFACE METHODS ================================//
//=====================================================================================================//

// (0)
void setMuxesPins_MUX_INFC ( void ) {
	char auxCharacter=0;
	int pin_A0_MUX_ABCDE=0, pin_A1_MUX_ABCDE=0, pin_A2_MUX_ABCDE=0, pin_EN_MUX_ABCDE=0;
	int pin_A0_MUX_ADC=0,   pin_A1_MUX_ADC=0,   pin_A2_MUX_ADC=0,   pin_EN_MUX_ADC=0;
	while (auxCharacter!=EXIT){
		iprintf("\n\n\n\nToggle Individual Pins used for mux addressing. E to exit\n");
		iprintf("(0) A0_MUX_ABCDE: %d\n", pin_A0_MUX_ABCDE);
		iprintf("(1) A1_MUX_ABCDE: %d\n", pin_A1_MUX_ABCDE);
		iprintf("(2) A2_MUX_ABCDE: %d\n", pin_A2_MUX_ABCDE);
		iprintf("(3) EN_MUX_ABCDE: %d\n", pin_EN_MUX_ABCDE);

		iprintf("(4) A0_MUX_ADC: %d\n", pin_A0_MUX_ADC);
		iprintf("(5) A1_MUX_ADC: %d\n", pin_A1_MUX_ADC);
		iprintf("(6) A2_MUX_ADC: %d\n", pin_A2_MUX_ADC);
		iprintf("(7) EN_MUX_ADC: %d\n", pin_EN_MUX_ADC);
		auxCharacter = sgetchar( 0 ); iprintf("%d\n", auxCharacter);
		switch (auxCharacter){
			case '0': pin_A0_MUX_ABCDE= !pin_A0_MUX_ABCDE; A0_MUX_ABCDE = pin_A0_MUX_ABCDE; break;
			case '1': pin_A1_MUX_ABCDE= !pin_A1_MUX_ABCDE; A1_MUX_ABCDE = pin_A1_MUX_ABCDE; break;
			case '2': pin_A2_MUX_ABCDE= !pin_A2_MUX_ABCDE; A2_MUX_ABCDE = pin_A2_MUX_ABCDE; break;
			case '3': pin_EN_MUX_ABCDE= !pin_EN_MUX_ABCDE; EN_MUX_ABCDE = pin_EN_MUX_ABCDE; break;
			case '4': pin_A0_MUX_ADC= !pin_A0_MUX_ADC; A0_MUX_ADC = pin_A0_MUX_ADC; break;
			case '5': pin_A1_MUX_ADC= !pin_A1_MUX_ADC; A1_MUX_ADC = pin_A1_MUX_ADC; break;
			case '6': pin_A2_MUX_ADC= !pin_A2_MUX_ADC; A2_MUX_ADC = pin_A2_MUX_ADC; break;
			case '7': pin_EN_MUX_ADC= !pin_EN_MUX_ADC; EN_MUX_ADC = pin_EN_MUX_ADC; break;
			case EXIT: iprintf ("Exiting...");
		}

	}
}

// (1)
void setMuxes_MUX_INFC ( void ) {
	char selectSamplFunc; int Num;
	iprintf("\n\n\n\n(1). Set Muxes for a reading\n");
	// sampling Function
	iprintf("Select Sampling Function (0-PSU_V, 1-PSU_I, 2-SUP_V):");
	selectSamplFunc = sgetchar( 0 ) - 0x30; iprintf("%d\n", selectSamplFunc);
	while (selectSamplFunc<0 || selectSamplFunc>2){
		iprintf ("Invalid value. Reselect sampling function: ");
		selectSamplFunc = sgetchar( 0 ) - 0x30; iprintf("%d\n", selectSamplFunc);
	}
	// psuSupNumber
	iprintf("Select %s number: ", (selectSamplFunc==2?"SUP":"PSU"));
	*bufferINFC = 0x00000000;Num=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while (Num<0 || (Num>=PSU_NUMBER && selectSamplFunc!=FUNCTION_SnI_VOLTAGE) || (Num>=SUP_NUMBER && selectSamplFunc==FUNCTION_SnI_VOLTAGE)){
		iprintf ("Invalid value. Reselect %s number: ", (selectSamplFunc==2?"PSU":"SnI"));
		Num = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	// Muxes' configuration
	setMUX(selectSamplFunc, Num);
	iprintf("Muxes set for sampling function %d and %s %d\n", selectSamplFunc, (selectSamplFunc==2?"SUP":"PSU"), Num);
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

	iprintf("\n SUPPLY VOLTAGES\n");
	iprintf(" SUP_42V_UNREG ( 0 )\n");
	iprintf(" SUP_35V_UNREG ( 1 )\n");
	iprintf(" SUP_16V_UNREG ( 2 )\n");
	iprintf(" SUP_32V_REG   ( 3 )\n");
	iprintf(" SUP_16V_REG   ( 4 )\n");
	iprintf(" SUP_12V_F_A   ( 5 )\n");
	iprintf(" SUP_12V_F_B   ( 6 )\n");
	iprintf(" SUP_12V_F_C   ( 7 )\n");
	iprintf(" SUP_n16_REG   ( 8 )\n");
	iprintf(" SUP_n20_UNREG ( 9 )\n");
	iprintf(" SUP_12V_F_D 	( 10 )\n");

	iprintf("  INTERNAL VOLTAGES\n");
	iprintf(" INT_VCC_3V3   ( 11 )\n");
	iprintf(" INT_VCC_12V   ( 12 )\n");
	iprintf(" INT_VCC_n12V  ( 13 )\n\n");

	iprintf("Select SnI number: ");
	*bufferINFC = 0x00000000;
	sniNum=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while (sniNum<0 || sniNum>=SUP_NUMBER){
		iprintf ("Invalid value. Reselect SnI number: ");
		sniNum = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	iprintf("Reading Voltage...");
	readVoltageValue(sniNum, SnI_TYPE_LIST);
	iprintf("Voltage for SnI %d: %s\n", sniNum, ftos(supList[sniNum].vOut));
}

// (5)
void readA2D_MUX_INFC ( void ){
	WORD a2dRes = (ReadA2DResult(0) >> 3);
	iprintf("\n\n\n\n(5). Read a2d\n");
	iprintf("Sampling Function: %d\n", getSamplingFunctionMUX());
	iprintf("Number: %d\n", getNumMUX());
	iprintf("Reading: %s  (0x%x in A2D = %s V)\n", ftos(ADCCountsToVoltORCurr(a2dRes, getSamplingFunctionMUX(), getNumMUX()), 2, 2, true), a2dRes, ftos(((a2dRes)/(4095.0)*(3.3)), 3, 1, true));
}

// (6)
void getMuxesStatus_MUX_INFC ( void ){
	iprintf("\n\n\n\n(6). Get Muxes' status\n");
	iprintf("Sampling Function: %d\n", getSamplingFunctionMUX());
	iprintf("Number: %d\n", getNumMUX());
}


