/*
 * I2CLibrary.h
 *
 *	Communications using I2C bus. Includes communications across a I2C to SPI converter, arriving into
 *	SPI buses (used further in the system to communicate with RDACs).
 *	Clarification: The SPI communication does not use the MFC-5213 in-built pins. No code developed for that purpose
 *
 *  Created on: 29-ene-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef LIB_I2CSPI
#define LIB_I2CSPI

#include "Headers.h"
#include "Libraries/GeneralLibrary.h"	// Functions with various general purposes


//=====================================I2C&SPI METHODS===========================================//
//*********Public Methods*********//
// I2C
BYTE sendI2CMessage ( BYTE outputBuffer[], uint bufSize, BYTE I2CAdress );
// SPI
BYTE configureSPI( BOOL order, BOOL cpol, BOOL cpha, BYTE clkRate, BYTE I2CtoSPIBridgeAddress );
BYTE sendSPImessage ( BYTE outputBuffer[], uint bufSize, BYTE slaveSelect, BYTE I2CtoSPIBridgeAddress );
BYTE readI2CBridgeBuffer ( BYTE inputBuffer[], uint bufSize, BYTE I2CtoSPIBridgeAddress);
// Address translation to PSU Number
int addr2PSUnum ( BYTE I2CAddress, BYTE SPIAddress );

//*********Private Methods*********//
// Auxiliary
void errorDetails( BYTE I2CStat );

#endif /* LIBRARY_H_ */

