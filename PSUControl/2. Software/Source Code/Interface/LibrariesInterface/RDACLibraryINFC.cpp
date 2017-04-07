/*
 * RDACLibraryINFC.cpp
 *
 *	Interface Menu and Methods for RDACLibrary
 *
 *  Created on: 22-jun-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods

//==============================================VARIABLES==============================================//
// Keyboard
#define WAIT_FOR_KEYBOARD funcChar = sgetchar(0);
extern char funcChar;				// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
extern char functionSelectionINFC;
extern char auxSelectionINFC;
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];

// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];					// MAIN PSU ARRAY LIST
extern SnI_TYPE supList [SUP_NUMBER];					// Supply & Internal voltages List

// I2C&SPI, RDAC
#define CHANGE_SLAVE slaveSPIAddressINFC=(psuNumINFC&0x1?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS);
extern BYTE slaveSPIAddressINFC;	// Set to UPPER_SLAVE_SPI_ADDRESS or LOWER_SLAVE_SPI_ADDRESS
extern BYTE i2cAddressINFC;			// Defined by 3 switches for RDACs. 0x28 to 0x2F

// Values
extern int newvalueINFC;
extern BOOL ctrl_allowINFC;


//=====================================================================================================//
//============================== PUBLIC RDAC LIBRARY INTERFACE MENU ===================================//
//=====================================================================================================//

void RDACLibraryINFC( void ){
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("=============================== RDAC LIBRARY ==================================\r\n" );
		iprintf( "I2C: 0x%x       SLAVE: %s, %d\n",  i2cAddressINFC, (slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?"LOWER":"UPPER"), slaveSPIAddressINFC);
		iprintf( "\nADDRESSING OPTIONS\r\n" );
		iprintf( " (+) Change I2C Address destination manually (0x2F to 0x28 for RDACs)\r\n" );
		iprintf( " (-) Toggle SPI Selected slave (Upper/Lower)\r\n" );
		iprintf( " (*) Set I2C & SPI addresses for a certain PSU\r\n" );
		iprintf( "\nRDAC FUNCTIONS (1 to C)\r\n" );
		iprintf( " (0) Change RDAC Value (Register)\r\n" );
		iprintf( " (1) Change RDAC Value (Volts)\r\n" );
		iprintf( " (2) Read RDAC Value\r\n" );
		iprintf( " (3) Change RDAC Control Register (allow/reject updates)\r\n" );
		iprintf( " (4) Read RDAC Control Register\r\n" );
		iprintf( " (5) Reset RDAC\r\n" );
		iprintf( " (6) High Impedance on SDO for RDAC\r\n" );
		iprintf( " (7) Scaled (+1,-1) change of RDAC value\r\n" );
		iprintf( " (8) Program RDAC Value in 20TP-Mem\r\n" );
		iprintf( " (9) Read RDAC 20TP-Mem Value\r\n" );
		iprintf( " (A) Convert Volts to RDAC Counts\n");
		iprintf( " (B) Convert RDAC Counts to Volts \n\n");
		iprintf( " (C) Hardware RESET of all RDACs \n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '0': 	setRDACValueReg_RDAC_INFC(); 			break;
			case '1': 	setRDACValueVolts_RDAC_INFC(); 			break;
			case '2':	readRDACValue_RDAC_INFC(); 				break;
			case '3':	toggleRDACCtrl_RDAC_INFC(); 			break;
			case '4':	getRDACCtrl_RDAC_INFC(); 				break;
			case '5':	resetRDAC_RDAC_INFC(); 					break;
			case '6':	higImpSDO_RDAC_INFC(); 					break;
			case '7':	scaleSetRDACValue_RDAC_INFC(); 			break;
			case '8':	programMemRDAC_RDAC_INFC(); 			break;
			case '9':	readMemRDAC_RDAC_INFC(); 				break;
			case 'a': case 'A':	voltsToCounts_RDAC_INFC(); 		break;
			case 'b': case 'B':	countsToVolts_RDAC_INFC(); 		break;
			case 'c': case 'C': hardwareReset_RDAC_INFC();		break;
			case '+':	changeI2CAddress_I2CnSPI_INFC();		break;
			case '-':	toggleSPIAddress_I2CnSPI_INFC();		break;
			case '*':	setAddressesPSU_I2CnSPI_INFC();			break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\n", functionSelectionINFC);
				iprintf( " \nPRESS ANY KEY TO RETURN TO RDAC LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}



//=====================================================================================================//
//============================= PRIVATE RDAC LIBRARY INTERFACE METHODS ================================//
//=====================================================================================================//

// (0)
void setRDACValueReg_RDAC_INFC ( void ){
	iprintf("\n\n\n\n(0). Configuring RDAC Value in Counts\n");
	iprintf(" Enter the value (0x000 to 0x3ff) to be configured in RDAC Register:\n");
	*bufferINFC = 0x0000;
	newvalueINFC = (WORD)Ascii2Hex( gets((char*)bufferINFC),3);iprintf("\n");
	setValRDAC(newvalueINFC, slaveSPIAddressINFC, i2cAddressINFC);
}

// (1)
void setRDACValueVolts_RDAC_INFC ( void ){
	float outVolt; uint psuNum;
	iprintf("\n\n\n\n(1). Configuring RDAC Value in Volts\n");
	psuNum=addr2PSUnum(i2cAddressINFC, slaveSPIAddressINFC);
	if(psuNum<0){iprintf("I2C value not selected for any PSU\n"); return;}

	if		(i2cAddressINFC==0x2A) {
		outVolt = scanNegativeFloatValue();
		newvalueINFC = voltNegativeToCounts(outVolt, psuList[psuNum].rShunt );}

	else if (i2cAddressINFC<=0x2F && i2cAddressINFC>=0x2B) {
		outVolt = scanPositiveFloatValue();
		newvalueINFC = voltPositiveToCounts(outVolt, psuList[psuNum].rShunt );}
	setValRDAC(newvalueINFC, slaveSPIAddressINFC, i2cAddressINFC);
}

// (2)
void readRDACValue_RDAC_INFC ( void ){
	iprintf("\n\n\n\n(2). Reading RDAC Value\n");
	getValRDAC(slaveSPIAddressINFC, i2cAddressINFC);
}

// (3)
void toggleRDACCtrl_RDAC_INFC ( void ){
	ctrl_allowINFC = !ctrl_allowINFC;
	if(ctrl_allowINFC){
		iprintf("\n\n\n\n(3). Configuring control register to ALLOW RDAC Value to be updated\n");
		setCtrlRDAC(0,1,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (1)
	}
	else{
		iprintf("\n\n\n\n(3). Configuring control register to REJECT RDAC Value updates\n");
		setCtrlRDAC(0,0,0, slaveSPIAddressINFC, i2cAddressINFC);	// Second bit controls RDAC update permission (0)
	}
}

// (4)
void getRDACCtrl_RDAC_INFC ( void ){
	iprintf("\n\n\n\n(4). Reading RDAC Ctrl Register\n");
	getCtrlRDAC(slaveSPIAddressINFC, i2cAddressINFC);
}

// (5)
void resetRDAC_RDAC_INFC ( void ){
	iprintf("\n\n\n\n(5). Resetting RDAC to initial value\n");
	resetRDAC(slaveSPIAddressINFC, i2cAddressINFC);
}

// (6)
void higImpSDO_RDAC_INFC ( void ){
	iprintf("\n\n\n\n(6). Setting RDAC SDO in high impedance\n");
	highImpRDAC(slaveSPIAddressINFC, i2cAddressINFC);
}

// (7)
void scaleSetRDACValue_RDAC_INFC ( void ){
	newvalueINFC = getValRDAC(slaveSPIAddressINFC, i2cAddressINFC);
	config_consoleOutput_RDAC ( OFF );
	config_consoleOutput_I2CnSPI ( OFF );

	iprintf("newvalue = %d\n", newvalueINFC);

	iprintf( "\n\n\n\n(7). Scaled change RDAC value\r\n Command list:\n" );
	iprintf( " (+) More Output Voltage\r\n" );
	iprintf( " (-) Less Output Voltage\r\n" );
	iprintf( " (e) Exit\r\n" );
	iprintf( "\r\nEnter command \r\n >" );
	auxSelectionINFC =sgetchar (0);
	while ( auxSelectionINFC != EXIT ){
		switch ( auxSelectionINFC ){
			case '+':
				newvalueINFC-=1;
				setValRDAC(newvalueINFC, slaveSPIAddressINFC, i2cAddressINFC);
				printf( "Output Voltage +: %.2f (n=%d)\r\n", countsToVoltPositive(newvalueINFC, 820), newvalueINFC);
				break;

			case '-':
				newvalueINFC+=1;
				setValRDAC(newvalueINFC, slaveSPIAddressINFC, i2cAddressINFC);
				printf( "Output Voltage -: %.2f (n=%d)\r\n", countsToVoltPositive(newvalueINFC, 820), newvalueINFC);
				break;
			case 'e':break;
			default:iprintf( "INVALID COMMAND -> %c\r\n", auxSelectionINFC);break;
		}
		iprintf("\n > ");
		auxSelectionINFC =sgetchar (0);
	}
	iprintf("\n > Exiting Scaled change RDAC\n");
	config_consoleOutput_RDAC ( OFF );
	config_consoleOutput_I2CnSPI ( OFF );
}

// (8)
void programMemRDAC_RDAC_INFC ( void ){
	iprintf( "\n\n\n\n(8) Program RDAC Value in 20TP-Mem\r\n" );
	auxSelectionINFC = 'n';
	while (auxSelectionINFC!='y'){
		iprintf( " Enter the value to be programmed (in HEX, values from 0 to 3ff):\r\n" );
		*bufferINFC = 0x0000;
		newvalueINFC = (WORD)Ascii2Hex( gets((char*)bufferINFC),3);iprintf("\n");
		iprintf( " Do you wish to program value 0x%x in slave %d with I2C = 0x%x ? (y/n)\r\n", newvalueINFC, slaveSPIAddressINFC, i2cAddressINFC);
		auxSelectionINFC =sgetchar(0);
		if(auxSelectionINFC==EXIT){iprintf("Exiting...\n");return;}
	}
	programMemRDAC(newvalueINFC,slaveSPIAddressINFC, i2cAddressINFC );
}

// (9)
void readMemRDAC_RDAC_INFC ( void ){
	iprintf( "\n\n\n\n(9) Read RDAC 20TP-Mem Value\r\n" );
	readMemRDAC(slaveSPIAddressINFC, i2cAddressINFC );
}

// (A)
void voltsToCounts_RDAC_INFC ( void ){
	float volt; int rshunt;
	iprintf( "\n\n\n\n(A). Convert Volts to RDAC Counts\n");
	iprintf(" Enter RDAC counts: ");
	volt = atof(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf(" Enter Rshunt: ");
	rshunt = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf(" Converted Counts: %s\n", voltPositiveToCounts(volt, rshunt));
}

// (B)
void countsToVolts_RDAC_INFC ( void ){
	int rdacCounts, rshunt;
	iprintf( "\n\n\n\n(B). Convert RDAC Counts to Volts\n");
	iprintf(" Enter RDAC counts: ");
	rdacCounts = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf(" Enter Rshunt: ");
	rshunt = atoi(gets((char*)bufferINFC));iprintf("\r\n");
	iprintf(" Converted Volts: %s\n", countsToVoltPositive(rdacCounts, rshunt));
}

// (C)
void hardwareReset_RDAC_INFC ( void ){
	iprintf( "\n\n\n\n(C). Hardware RESET of all RDACs \n\n");
	iprintf( "Reseting all RDACs...\n.....................\n");
	RESET_RHEOSTATS = 0; OSTimeDly(TICKS_PER_SECOND);RESET_RHEOSTATS = 1;
	iprintf( "All Devices Reseted\n");


}
