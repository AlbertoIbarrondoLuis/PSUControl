/*
 * RDACLibrary.h
 *
 *	Configuration and Reading of Digital Rheostats AD5292 using I2C&SPILibrary
 *	For further inquiries please refer to AD5292 Datasheet
 *
 *  Created on: 02-feb-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef LIB_RDAC
#define LIB_RDAC

#include "Headers.h"
#include "Libraries/GeneralLibrary.h"	// Functions with various general purposes
#include "Libraries/I2C&SPILibrary.h"	// I2C&SPI Communication

//====================================RDAC METHODS===========================================//
//*********Public Methods*********//
// RDAC Configuration and Reading
void setCtrlRDAC(BOOL calibrationDisable, BOOL regWriteEnable, BOOL programEnable, BYTE slaveSelect, BYTE I2CAdress);
BYTE getCtrlRDAC(int slaveSelect, BYTE I2CAdress);
void setValRDAC(int value, BYTE slaveSelect, BYTE I2CAddress);
int getValRDAC(int slaveSelect, BYTE I2CAddress);
void highImpRDAC(int slaveSelect, BYTE I2CAddress);
void resetRDAC(int slaveSelect, BYTE I2CAddress);
void shutdownRDAC(BOOL shutdownModeOn,int slaveSelect, BYTE I2CAddress);
void donothingRDAC(BYTE ibuf[], int slaveSelect, BYTE I2CAddress);
void programMemRDAC(int value, BYTE slaveSelect, BYTE I2CAddress);
WORD readMemRDAC(BYTE slaveSelect, BYTE I2CAddress);

// Auxiliary Methods
BYTE getI2CResultRDAC(void);
void printValRDAC (BYTE ibuf[]);
void printCtrlRDAC (BYTE ibuf[]);
int voltToCounts (float volt, float Rpsu);
float countsToVolt (int counts, float Rpsu);
int scanFloatValue ( float Rpsu );


//*********Private Methods*********//
// All the messages are stored in the given buffer's first 2 Bytes.
void doNothingCOM(BYTE* buffer);
void writeRegCOM(BYTE* buffer, int value);
void readRegCOM(BYTE* buffer);
void writeMemCOM(BYTE* buffer);
void storeRegtoMemCOM(BYTE* buffer);
void resetCOM(BYTE* buffer);
void readMemCOM(BYTE* buffer, BYTE dir);
void writeCtrlCOM(BYTE* buffer, BOOL programEnable, BOOL regWriteEnable, BOOL calibrationDisable);
void readCtrlCOM(BYTE* buffer);
void shutdownCOM(BYTE* buffer, BOOL shutdownModeOn);
void highImpCOM(BYTE* buffer);

#endif /* RDACCOMPLEXLIBRARY_H_ */
