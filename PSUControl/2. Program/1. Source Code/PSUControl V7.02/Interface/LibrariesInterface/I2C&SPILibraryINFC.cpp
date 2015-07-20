/*
 * I2CnSPIFunctions.cpp
 *
 *	Interface Menu and Methods for I2C&SPILibrary
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


// I2C&SPI, RDAC
#define CHANGE_SLAVE slaveSPIAddressINFC=(psuNumINFC&0x1?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS);
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];
extern BYTE slaveSPIAddressINFC;	// Set to UPPER_SLAVE_SPI_ADDRESS or LOWER_SLAVE_SPI_ADDRESS
extern BYTE i2cAddressINFC;			// Defined by 3 switches for RDACs. 0x28 to 0x2F
extern BYTE bridgeI2CAddressList[PSU_NUMBER];

// Values
extern int psuNumINFC;


//=====================================================================================================//
//========================== PUBLIC I2C & SPI LIBRARY INTERFACE MENU ==================================//
//=====================================================================================================//

void I2CnSPILibraryINFC( void ){
	functionSelectionINFC = '0';
	while ( functionSelectionINFC!=EXIT ){
		ERASE_CONSOLE
		iprintf("============================= I2C&SPI LIBRARY ===============================\r\n" );
		iprintf( "I2C: 0x%x       SLAVE: %s, %d\n",  i2cAddressINFC, (slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?"UPPER":"LOWER"), slaveSPIAddressINFC);
		iprintf( "\nADDRESSING OPTIONS\r\n" );
		iprintf( " (+) Change I2C Address destination manually (0x2F to 0x28 for RDACs)\r\n" );
		iprintf( " (-) Toggle SPI Selected slave (Upper/Lower)\r\n" );
		iprintf( " (*) Set I2C & SPI addresses for a certain PSU\r\n" );
		iprintf( "\nI2C&SPI FUNCTIONS\r\n" );
		iprintf( " (1) Configure both I2C & SPI Channels\n");
		iprintf( " (2) Send I2C configurable message\n");
		iprintf( "\n (e) EXIT TO LIBRARIES MENU \r\n" );
		iprintf("-------------------------------------------------------------------------------\r\n" );
		iprintf( "\r\nEnter command: " );
		functionSelectionINFC = sgetchar(0);iprintf("%c\n", functionSelectionINFC);
		iprintf("\n-------------------------------------------------------------------------------\r\n" );
		switch ( functionSelectionINFC ){
			case '1':configChannels_I2CSPI_INFC(); 		break;
			case '2':sendI2CMessage_I2CnSPI_INFC();		break;
			case '+':changeI2CAddress_I2CnSPI_INFC();	break;
			case '-':toggleSPIAddress_I2CnSPI_INFC();	break;
			case '*':setAddressesPSU_I2CnSPI_INFC(); 	break;
			case EXIT: iprintf("Exiting to LIBRARIES MENU\n");  break;
			default:
				iprintf( "\nINVALID KEY -> %c\r\n", functionSelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO I2C&SPI LIBRARY\r\n" );
				break;
		}
		WAIT_FOR_KEYBOARD
	}
}



//=====================================================================================================//
//========================= PRIVATE I2C & SPI LIBRARY INTERFACE METHODS ===============================//
//=====================================================================================================//

// (1)
void configChannels_I2CSPI_INFC( void ){
	iprintf("\n\n\n\n(1). Configure both I2C & SPI Channels\n");
	I2CInit(MCF5213_I2C_ADDRESS, I2C_FREQUENCY);
	configureSPI( false, false, false, 2, i2cAddressINFC);	// MSB first, CLK low when idle, data clocked
}

// (2)
void sendI2CMessage_I2CnSPI_INFC( void ){
	iprintf("\n\n\n\n(2). Send I2C configurable message\n");
	I2CInit(MCF5213_I2C_ADDRESS, I2C_FREQUENCY);
	iprintf("Buffer to TX (2B = 4 HEX characters): ");
	gets((char*)bufferINFC);
	sendI2CMessage ( bufferINFC, (strlen( (const char*)bufferINFC ) )+1, i2cAddressINFC);
}

// (+)
void changeI2CAddress_I2CnSPI_INFC( void ){
	iprintf( "(+). Change I2C Address destination manually (0x2F to 0x28 for RDACs)\r\n" );
	i2cAddressINFC = 0;
	while( (i2cAddressINFC < 0x07) || (i2cAddressINFC > 0x78) )  //While address is invalid
	{
	   iprintf( "Enter valid 7-bit (2-digit hex) destination address for Master Transmit: 0x");
	   *bufferINFC = 0x0000;
	   i2cAddressINFC = (BYTE)Ascii2Hex( gets((char*)bufferINFC),2);
	   iprintf("\r\n");
	}
	iprintf(" -> Destination I2C Address: 0x%x\n", i2cAddressINFC);
}

// (-)
void toggleSPIAddress_I2CnSPI_INFC( void ){
	iprintf( "(-). Toggle SPI Selected slave (Upper/Lower)\r\n" );
	slaveSPIAddressINFC=(slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS);
	iprintf(" -> Selected %s PSU (SPI dir: 0x%x)\n", (slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?"UPPER":"LOWER"), slaveSPIAddressINFC );
}

// (*)
void setAddressesPSU_I2CnSPI_INFC( void ){
	iprintf( "(*). Set I2C & SPI addresses for a certain PSU\r\n" );
	psuNumINFC = selectPSUNumINFC();
	i2cAddressINFC = bridgeI2CAddressList[psuNumINFC];
	slaveSPIAddressINFC=(psuNumINFC&0x1?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS);
	iprintf(" -> Selected PSU: %d\n", psuNumINFC);
	iprintf(" -> Destination I2C Address: 0x%x\n", i2cAddressINFC);
	iprintf(" -> Selected %s PSU (SPI dir: 0x%x)\n", (slaveSPIAddressINFC==LOWER_SLAVE_SPI_ADDRESS?"UPPER":"LOWER"), slaveSPIAddressINFC );
}
