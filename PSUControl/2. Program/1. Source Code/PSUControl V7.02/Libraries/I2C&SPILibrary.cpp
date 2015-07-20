/*
 * I2CLibrary.cpp
 *
 *	Communications using I2C bus. Includes communications across a I2C to SPI converter, arriving into
 *	SPI buses (used further in the system to communicate with RDACs).
 *	Clarification: The SPI communication does not use the MFC-5213 in-built pins. No code developed for that purpose
 *
 *  Created on: 29-ene-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Libraries/I2C&SPILibrary.h"

//===========================================VARIABLES=================================================//
// I2C communication result
BYTE I2CStat;

// Console output - Configurable in ConfigCTRL.cpp
extern BOOL consoleOutputI2C;			// Toggles console messages for I2C and SPI functions

// Auxiliary buffers to send and receive messages
BYTE sendBuffer[I2C_MAX_BUF_SIZE];   	// User created I2C output buffer
BYTE* pSendBuf = sendBuffer;         	// Pointer to user I2C output buffer
BYTE receiveBuffer[I2C_MAX_BUF_SIZE];   // User created I2C output buffer
BYTE* pRecBuf = receiveBuffer;         	// Pointer to user I2C output buffer



//=====================================================================================================//
//=======================================    I2C METHODS    ===========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// sendI2CMessage - sends the bufSize Bytes from outputBuffer to I2CAdress through I2C including a null
//					at the end. To do so, it uses Multi-master I2C communications included in multi.h
//
// 		#INPUT: BYTE outputBuffer[] - buffer containing the message to be sent
//				int bufSize - number of bytes to b transmitted from outputBuffer
//				BYTE I2CAdress -
// 		#OUTPUT: BYTE I2CStat
//		>E.G.: minAGC() - AGCLibraryINFC.cpp
//-------------------------------------------------------------------------------------------------------
BYTE sendI2CMessage ( BYTE outputBuffer[], uint bufSize, BYTE I2CAdress){
		if(consoleOutputI2C){		iprintf( "I2C msg (0x%x) -> ", I2CAdress );
									printBuffer(outputBuffer, bufSize);
		}
		I2CStat = I2CSendBuf(I2CAdress, outputBuffer, bufSize);
		if( I2CStat == I2C_OK ){
			if(consoleOutputI2C){	iprintf( " -> I2C OK\n" );
			}
		}
		else {
			if(consoleOutputI2C){	iprintf( " -> I2C ERROR:%d\r\n", I2CStat);
									errorDetails(I2CStat);
			}
		}
		return I2CStat;
}


//-------------------------------------------------------------------------------------------------------
// configureSPI - sends 2 bytes using outputBuffer to configure the SPI channel for the selected I2CAddress
//					(which should be the address of a I2C to SPI Bridge from a SlotCard). For further info,
//					please refer to SC18IS602B Datasheet.
// 		#INPUT: BOOL order - MSB/LSB first (set always to 0 to send MSB first) (0/1)
//				BOOL cpol - Clock Polarity: HIGH/LOW when idle (0/1)
//				BOOL cpha - Clock Phase: data clocked in on leading/trailing edge (0/1)
//				BYTE clkRate - 0 to 3. 115KHz as default (higher values resulted on bugs). Correspondence:
//								0 -> 1.8MHz; 1 -> 461KHz; 2 -> 115KHz; 3 -> 58KHz;
//				BYTE I2CtoSPIBridgeAddress - I2C address of the I2CtoSPI bridge.
// 		#OUTPUT: BYTE I2CcomResult - Result of I2C communication. I2C_OK means correctly sent
//		>E.G.: configureSPI( false, false, false, 2, i2CtoSPIAddress[i] ); - initialized in main.cpp
//-------------------------------------------------------------------------------------------------------
BYTE configureSPI( BOOL order, BOOL cpol, BOOL cpha, BYTE clkRate, BYTE I2CtoSPIBridgeAddress){
		BYTE configureSPI[2] = {0xF0, 0x20 * order + 0x08 * cpol + 0x04 * cpha+ (((clkRate < 4) && (clkRate > 0))?clkRate:0)};
		BYTE I2CcomResult = sendI2CMessage( configureSPI, 2, I2CtoSPIBridgeAddress);
		if (I2CcomResult==0){
			if(consoleOutputI2C){
				iprintf( " .Configured SPI channel:\r\n" );
				iprintf( "   -   Byte order: %s\n", (order?"LSB of data transmitted first": "MSB of data transmitted first"));
				iprintf( "   -   Clock polarity: %s\n", (order?"SPI Clock HIGH when idle": "SPI Clock HIGH when idle"));
				iprintf( "   -   Clock Phase: %s\n", (order?"Data clocked in on trailing edge": "Data clocked in on leading edge"));
			}
		}
		else{
			if(consoleOutputI2C){
				iprintf( " SPI channel couldn't be configured\r\n" );
			}
		}
		return I2CcomResult;

}


//-------------------------------------------------------------------------------------------------------
// sendSPImessage - sends bufSize Bytes from the BufSPI buffer to the selected SPI Slaves.
// 		#INPUT: BYTE outputBuffer[] - Buffer containing the Bytes to be sent.
//				uint bufSize - Number of bytes to be sent from outputBuffer
//				BYTE slaveSelect - 0x0 to 0xF possible, each bit controls one Slave (0x1, 0x2, 0x4, 0x8)
//				BYTE I2CtoSPIBridgeAddress - I2C address of the I2CtoSPI bridge.
// 		#OUTPUT: BYTE result - Result of I2C communication. I2C_OK means correctly sent
//		>E.G.: sendSPImessage (outputBuffer, 2, 0x2, 0x2F) - used in RDACLibrary.cpp
//-------------------------------------------------------------------------------------------------------
BYTE sendSPImessage ( BYTE outputBuffer[], uint bufSize, BYTE slaveSelect, BYTE I2CtoSPIBridgeAddress){
		if (slaveSelect>0x0F){
			iprintf("SPI ERROR: Wrong SPI address");
			return 0xF;	// Not important, just a value different from any I2C Result (including I2C_OK)
		}
		else{
			*pSendBuf++=slaveSelect;
			mergeBuffer(pSendBuf, outputBuffer, bufSize);
			pSendBuf = sendBuffer;
			if(consoleOutputI2C){		iprintf("SPI msg (Sl:%d) --> ", slaveSelect);}
			BYTE I2CcomResult = sendI2CMessage( sendBuffer, bufSize + 1 , I2CtoSPIBridgeAddress);
			return I2CcomResult;
		}
}


//-------------------------------------------------------------------------------------------------------
// readI2CBridgeBuffer - reads the bufSize Bytes from the I2CtoSPIBridge buffer and writes into inputBuffer
// 		#INPUT: BYTE inputBuffer[] - Buffer where the read bytes will be written.
//				uint bufSize - Number of bytes to be read
//				BYTE I2CtoSPIBridgeAddress - I2C address of the I2CtoSPI bridge.
// 		#OUTPUT: BYTE I2CStat - Result of I2C communication. I2C_OK means correctly sent.
//								errorDetails(I2CStat) will print by console the error cause
//		>E.G.: readI2CBridgeBuffer(ibuf, 2, I2CAddress); - used in doNothingRDAC, in RDACLibrary.cpp
//-------------------------------------------------------------------------------------------------------
BYTE readI2CBridgeBuffer ( BYTE inputBuffer[], uint bufSize, BYTE I2CtoSPIBridgeAddress){
		if(consoleOutputI2C){	iprintf( "Reading bridge buffer\n" );}
		I2CStat = I2CReadBuf(I2CtoSPIBridgeAddress, inputBuffer, bufSize);
		if( I2CStat == I2C_OK )	{
			if(consoleOutputI2C){	iprintf("Data Received -> " );
									printBuffer(inputBuffer, bufSize);
									iprintf(" -> I2C OK\r\n" );
			}
		}
		else{
			if(consoleOutputI2C){	iprintf("I2C ERROR: Failed to read due to error: %d\r\n", I2CStat);
									errorDetails(I2CStat);
			}
		}
		return I2CStat;
}





//=====================================================================================================//
//===================================  PRIVATE I2C&SPI METHODS  =======================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// errorDetails - prints on Console the details of an error ID. For further inquiries refer to i2cmulti.h
// 		#INPUT: BYTE I2CStat - I2C Result code
// 		#OUTPUT: None
//		>E.G.: errorDetails(I2CStat); - used in readI2CBridgeBuffer() right above
//-------------------------------------------------------------------------------------------------------
void errorDetails(BYTE I2CStat){
	switch (I2CStat){
		case 1:	iprintf( " ~Description: I2C bus is OK for a write\n" ); 													break;
		case 2:	iprintf( " ~Description: I2C bus is OK for a read\n");														break;
		case 3:	iprintf( " ~Description: I2C finished transmission but still owns but (need to stop or restart)\n");		break;
		case 4:	iprintf( " ~Description: A timeout occurred while trying communicate on I2C bus\n");						break;
		case 5:	iprintf( " ~Description: A timeout occurred while trying gain I2C bus control\n");							break;
		case 6:	iprintf( " ~Description: A read or write was attempted before I2C ready or during a slave transmission\n");	break;
		case 7:	iprintf( " ~Description: Lost arbitration during start\n");													break;
		case 8:	iprintf( " ~Description: Lost arbitration and then winner addressed our slave address\n");					break;
		case 9:	iprintf( " ~Description: Received no ACK from slave device\n");												break;
		default:iprintf( " ~Description: Unknown error Code\n" ); 															break;
	}
	iprintf("\n");
}
