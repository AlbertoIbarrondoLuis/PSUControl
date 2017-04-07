/*
 * RDACLibrary.cpp
 *
 *	Configuration and Reading of Digital Rheostats AD5292 using I2C&SPILibrary.
 *	For further inquiries please refer to AD5292 Datasheet
 *
 *  Created on: 02-feb-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "RDACLibrary.h"


//==============================================VARIABLES==============================================//
// Configuration
BOOL readBuffer = true;							// Controls I2C Buffer reading when using doNothingRDAC
extern BOOL consoleOutputRDAC;					// Toggles console messages for RDAC functions

// I2C&SPI
BYTE outputBuffer[5];							// Used to send every message
BYTE inputBuffer[5];							// Used to receive every message
BYTE resultI2C_RDAC;							// Result Code of SPI Message sending

// Keyboard
char floatScanBuffer[7];						// Scan float value




//=====================================================================================================//
//=====================================   PUBLIC RDAC METHODS   =======================================//
//=====================================================================================================//

//=================================  RDAC CONFIGURATION AND READING  ==================================//
//-------------------------------------------------------------------------------------------------------
// setCtrlRDAC - Stores the given flags into the RDAC Control Register
// 		#INPUT: BOOL calibrationDisable - RDAC Calibration (0-Default) or Normal (1) performance mode
//				BOOL regWriteEnable - Protects (0-default) or allows (1) RDAC Register update.
//				BOOL programEnable - Protects (0-default) or allows (1) 20TP Memory writing.
//				BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: setCtrlRDAC( false, false, false, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void setCtrlRDAC(BOOL calibrationDisable, BOOL regWriteEnable, BOOL programEnable, BYTE slaveSelect, BYTE I2CAddress){
	writeCtrlCOM(outputBuffer, calibrationDisable, regWriteEnable, programEnable);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	if (resultI2C_RDAC==I2C_OK){
		if(consoleOutputRDAC){iprintf("Configured Ctrl Reg %x in slave %d\n\n", outputBuffer[1], slaveSelect);}
	}
	else{
		if(consoleOutputRDAC){iprintf(" ~ERROR: Ctrl Register couldn't be CONFIGURED in slave %d for I2C 0x%x\n\n", slaveSelect, I2CAddress);}
	}
}


//-------------------------------------------------------------------------------------------------------
// getCtrlRDAC - Returns a BYTE with the RDAC Control Register last 3 bits
// 		#INPUT: BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT:.	0x04: RDAC Calibration (0-Default) or Normal (1) performance mode
//				.	0x02: Protects (0-default) or allows (1) RDAC Register update.
//				.	0x01: Protects (0-default) or allows (1) 20TP Memory writing.
//		>E.G.: getCtrlRDAC( 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
BYTE getCtrlRDAC(int slaveSelect, BYTE I2CAddress){
	if(I2CAddress!=AGC_I2C_ADDRESS){
		highImpRDAC((slaveSelect==UPPER_SLAVE_SPI_ADDRESS?LOWER_SLAVE_SPI_ADDRESS:UPPER_SLAVE_SPI_ADDRESS), I2CAddress);
	}
	readCtrlCOM(outputBuffer);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
	if(consoleOutputRDAC){
		if (resultI2C_RDAC==I2C_OK){
			printCtrlRDAC (inputBuffer);}		// Prints by console the contents of Ctrl Register
		else{
			printf(" ~ERROR: Ctrl Register couldn't be READ in slave %d for I2C 0x%x\n\n", slaveSelect, I2CAddress);
		}
	}
	return inputBuffer[1]&0xF;
}


//-------------------------------------------------------------------------------------------------------
// setValRDAC - Sets a new value to the RDAC Register (modifying its impedance). Value must be between 0
//				and 0x3FF.
// 		#INPUT: int value - 0x0 to 0x3ff. Updates RDAC Value Register (10 bits)
//				BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: setValRDAC( 0x200, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void setValRDAC(int value, BYTE slaveSelect, BYTE I2CAddress){
	setCtrlRDAC(0,1,0, slaveSelect, I2CAddress); // already set in init
	if(consoleOutputRDAC){ 	if (value<0)	{iprintf("Value set to RDAC_MAX_VALUE (%d)", RDAC_MAX_VALUE);}
							if (value>1023)	{iprintf("Value set to RDAC_MIN_VALUE (0)");}}
	value = (value<RDAC_MIN_VALUE?RDAC_MIN_VALUE:value);
	value = (value>RDAC_MAX_VALUE?RDAC_MAX_VALUE:value);
	writeRegCOM(outputBuffer, value);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	if(consoleOutputRDAC){
		if (resultI2C_RDAC==I2C_OK){
			iprintf("Configured value %d (0x%x) in slave nº%d\n\n", value, value, slaveSelect);
		}
		else{
			printf(" ~ERROR: Value Register couldn't be configured in slave %d for I2C 0x%x\n\n", slaveSelect, I2CAddress);
		}
	}
}


//-------------------------------------------------------------------------------------------------------
// getValRDAC - reads the RDAC Register, containing the impedance value.
// 		#INPUT: BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: int value - Value read in RDAC Value Register
//		>E.G.: getValRDAC(  0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
int getValRDAC(int slaveSelect, BYTE I2CAddress){
	if(I2CAddress!=AGC_I2C_ADDRESS){
		highImpRDAC((slaveSelect==UPPER_SLAVE_SPI_ADDRESS?LOWER_SLAVE_SPI_ADDRESS:UPPER_SLAVE_SPI_ADDRESS), I2CAddress);
	}
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
	readRegCOM(outputBuffer);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
	if(consoleOutputRDAC){// Prints by console the received value
		if (resultI2C_RDAC==I2C_OK){
			printValRDAC (inputBuffer);
		}
		else{
			printf(" ~ERROR: Value Register couldn't be READ in slave %d for I2C 0x%x\n\n", slaveSelect, I2CAddress);
		}
	}
	return (inputBuffer[0]&0x3)*0x100+inputBuffer[1];
}


//-------------------------------------------------------------------------------------------------------
/// highImpRDAC - Sets SDO (MISO pin) in high impedance. Must be used after every command not to interfere
//				 with other slaves.
// 		#INPUT: BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: highImpRDAC(  0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void highImpRDAC(int slaveSelect, BYTE I2CAddress){
	highImpCOM(outputBuffer);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect,I2CAddress);
	if(consoleOutputRDAC){
		if (resultI2C_RDAC ==I2C_OK){
			iprintf("High Impedance in slave %d\n\n", slaveSelect);
		}
		else {
			iprintf(" ~ERROR: High Impedance couldn't be configured in slave %d for I2C 0x%x\n\n", slaveSelect, I2CAddress);
		}
	}
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
}


//-------------------------------------------------------------------------------------------------------
// resetRDAC - Sets Ctrl Register to Default and impedance value to the one defined in 20TP-Mem
// 		#INPUT: BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: resetRDAC(  0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void resetRDAC(int slaveSelect, BYTE I2CAddress){
	resetCOM(outputBuffer);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
}


//-------------------------------------------------------------------------------------------------------
// shutdownRDAC - sets the device into Open Circuit. Doesn't always work
// 		#INPUT: BOOL shutdownModeOn - Open circuit
//				BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: shutdownRDAC( true, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void shutdownRDAC(BOOL shutdownModeOn,int slaveSelect, BYTE I2CAddress){
	shutdownCOM(outputBuffer, shutdownModeOn);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
}


//-------------------------------------------------------------------------------------------------------
// doNothingRDAC - empty command. Used to read a SPI response from the I2C Bridge buffer
// 		#INPUT: BYTE ibuf[] - buffer where 2 bytes are read
//				BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: shutdownRDAC( true, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void donothingRDAC(BYTE ibuf[], int slaveSelect, BYTE I2CAddress){
	doNothingCOM(outputBuffer);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	if(readBuffer){
		readI2CBridgeBuffer(ibuf, 2, I2CAddress);
	}
}


//-------------------------------------------------------------------------------------------------------
// programMemRDAC - sets RDAC 20TP-Mem to the desired value, checking first for correct value in RDAC Reg.
// 		#INPUT: int value - Value to be programmed
//				BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: None
//		>E.G.: shutdownRDAC( true, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
void programMemRDAC(int value, BYTE slaveSelect, BYTE I2CAddress){
	setValRDAC(value, slaveSelect, I2CAddress);
	if(getI2CResultRDAC()==I2C_OK){
		if (getValRDAC(slaveSelect, I2CAddress)==value && getI2CResultRDAC()==I2C_OK){
			iprintf(" 20TP programming\n");
			setCtrlRDAC(0, 1, 1, slaveSelect, I2CAddress);
			OSTimeDly(TICKS_PER_SECOND);
			writeMemCOM(outputBuffer);
			resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
			OSTimeDly(TICKS_PER_SECOND);
		}
		else{
			iprintf(" ERROR: RDAC value wasn't the desired one, programming aborted\n");
		}
	}
	OSTimeDly(TICKS_PER_SECOND * 4);
	getCtrlRDAC(slaveSelect, I2CAddress);
	setCtrlRDAC(0, 1, 0, slaveSelect, I2CAddress);
}


//-------------------------------------------------------------------------------------------------------
// readMemRDAC - Reads last programmed memory value
// 		#INPUT: BYTE slaveSelect - UPPER_SLAVE_SPI_ADDRESS/LOWER_SLAVE_SPI_ADDRESS - SPI slave address
//				BYTE I2CAddress - I2C Address of I2CtoSPIBridge
// 		#OUTPUT: WORD progValue - last programmed memory value
//		>E.G.: shutdownRDAC( true, 0x1, 0x2F );
//-------------------------------------------------------------------------------------------------------
WORD readMemRDAC(BYTE slaveSelect, BYTE I2CAddress){
	highImpRDAC((slaveSelect==UPPER_SLAVE_SPI_ADDRESS?LOWER_SLAVE_SPI_ADDRESS:UPPER_SLAVE_SPI_ADDRESS), I2CAddress);
	readMemCOM(outputBuffer, 0x14);
	resultI2C_RDAC = sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
	int dir = hammWeight((WORD)((inputBuffer[0]&0x7)*0x100+inputBuffer[1]));
	readMemCOM(outputBuffer, 0x15);
	resultI2C_RDAC |= sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
	donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
	dir += hammWeight((WORD)((inputBuffer[0]&0x7)*0x100+inputBuffer[1]));
	if (dir == 0){ // memory not programmed
		if(consoleOutputRDAC){iprintf (" readMemRDAC-> Memory not programmed");}
	}
	else{ // sent to last programmed memory
		if(consoleOutputRDAC){iprintf (" readMemRDAC-> Last programmed address: 0x%x\n", dir -1);}
		readMemCOM(outputBuffer, dir -1);
		resultI2C_RDAC |= sendSPImessage (outputBuffer, 2, slaveSelect, I2CAddress);
		donothingRDAC(inputBuffer, slaveSelect, I2CAddress);
		if(consoleOutputRDAC){iprintf ("20-TP Programmed"); printValRDAC(inputBuffer);}
	}
	return (WORD)(inputBuffer[0]*0x100+inputBuffer[1]);
}





//=====================================    AUXILIARY METHODS    =======================================//
//-------------------------------------------------------------------------------------------------------
// Getter resultI2C_RDAC
//-------------------------------------------------------------------------------------------------------
BYTE getI2CResultRDAC(void){	return resultI2C_RDAC;	}


//-------------------------------------------------------------------------------------------------------
// Print Value and Control Register from a 2-Byte buffer
//-------------------------------------------------------------------------------------------------------
void printValRDAC (BYTE ibuf[]){
	printf("Resistor Value: %d (0x%x in hex)\n", ((ibuf[0]&0x3)*0x100 + ibuf[1]), ((ibuf[0]&0x3)*0x100 + ibuf[1]));
}
void printCtrlRDAC (BYTE ibuf[]){
	printf(" Ctrl Rdac Contents:\n");
	printf("  - 20TP programming -> %s\n", (((ibuf[1]&0x01)>0)?"Enabled": "Disabled(default)"));
	printf("  - Register programming -> %s\n", (((ibuf[1]&0x02)>0)?"Enabled": "Disabled(default)"));
	printf("  - Resistor mode -> %s\n", (((ibuf[1]&0x04)>0)?"Normal": "Performance(default)"));
	printf("  - Mem Programming -> %s\n", (((ibuf[1]&0x08)>0)?"Successful": "Unsuccessful"));
}


//-------------------------------------------------------------------------------------------------------
// Conversion between RDAC Counts and Volts
//-------------------------------------------------------------------------------------------------------
int   voltPositiveToCounts 	(float volt, float Rpsu){	return (int)((1-((((float)volt/1.25)-1)*Rpsu/20000))*1023);	}// TODO: calibration
float countsToVoltPositive 	(int counts, float Rpsu){	return 1.25*(1+20000*(1-(float)counts/1023)/Rpsu);			}

int   voltNegativeToCounts 	(float volt, float Rpsu){	return (int)((1-((((float)volt/-2.4)-1)*Rpsu/20000))*1023);	}
float countsToVoltNegative 	(int counts, float Rpsu){	return -2.4*(1+20000*(1-(float)counts/1023)/Rpsu);			}

float scanPositiveFloatValue ( void ){
	iprintf(" Enter the new voltage value (ej.: 10.83) -> ");
	float num = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
	if(floatScanBuffer[0]==EXIT){iprintf("Exiting...\n");return -1;}
	while (num < 1.25 || num > 32){
		iprintf(" ERROR: a value between 1.26V and 32V must be given \n");
		iprintf(" Reenter the new voltage value (ej.: 5.42) -> ");
		num = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		if(floatScanBuffer[0]==EXIT){iprintf("Exiting...\n");return -1;}
	}
	return num;
}



float scanNegativeFloatValue ( void ){
	iprintf(" Enter the new voltage value (ej.: -10.83) -> ");
	float num = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
	if(floatScanBuffer[0]==EXIT){iprintf("Exiting...\n");return -1;}
	while (num < -16 || num > -2.4){
		iprintf(" ERROR: a value between -16V and -2.4V must be given \n");
		iprintf(" Reenter the new voltage value (ej.: -5.42) -> ");
		num = atof(gets((char *)floatScanBuffer));iprintf("\r\n");
		if(floatScanBuffer[0]==EXIT){iprintf("Exiting...\n");return -1;}
	}
	return num;
}




//=====================================================================================================//
//======================================   PRIVATE RDAC METHODS   =====================================//
//=====================================================================================================//

//----------------------------------------    BUFFER METHODS    ---------------------------------------//
// All the messages are stored in the given buffer's first 2 Bytes.


//-------------------------------------------------------------------------------------------------------
// donothing - empty command. Used to advance in reading
//-------------------------------------------------------------------------------------------------------
void doNothingCOM(BYTE* buffer){
	BYTE donothing[2] = {0x00, 0x00};
	mergeBuffer(buffer, donothing, 2);
}


//-------------------------------------------------------------------------------------------------------
// writeReg - command to write a value to the RDAC register. From 0x000 to 0x3FF (0 to 1023).
//-------------------------------------------------------------------------------------------------------
void writeRegCOM(BYTE* buffer, int value){
	if (value >0x3FF){
		value=0x200;
	}
	BYTE writeValue[2] = {0x04+((value & 0x300)>>8), (value & 0x0FF)};
	mergeBuffer(buffer, writeValue, 2);
}


//-------------------------------------------------------------------------------------------------------
// readReg - command to send the RDAC register (containing the resistor value) to the Bridge Buffer
//-------------------------------------------------------------------------------------------------------
void readRegCOM(BYTE* buffer){
	BYTE readValue[2] = {0x08, 0x00};
	mergeBuffer(buffer, readValue, 2);
}


//-------------------------------------------------------------------------------------------------------
// writeMem - command to write the contents of RDAC Reg into 20TP-Mem.
//-------------------------------------------------------------------------------------------------------
void writeMemCOM(BYTE* buffer){
	BYTE writeMem[2] = {0x0C, 0x00};
	mergeBuffer(buffer, writeMem, 2);
}


//-------------------------------------------------------------------------------------------------------
// reset -  command to restart the RDAC with a value of 0x200 (midscale) or the last programmed value
//-------------------------------------------------------------------------------------------------------
void resetCOM(BYTE* buffer){
	BYTE reset[2] = {0x10, 0x00};
	mergeBuffer(buffer, reset, 2);
}


//-------------------------------------------------------------------------------------------------------
// readMem - command to send the RDAC 20TP-Mem (containing the programmed value) to the Bridge Buffer
//-------------------------------------------------------------------------------------------------------
void readMemCOM(BYTE* buffer, BYTE dir){
	BYTE readDir[2] = {0x14, dir};
	mergeBuffer(buffer, readDir, 2);
}


//-------------------------------------------------------------------------------------------------------
// writeCtrl - command to set each bit of the Control register
//-------------------------------------------------------------------------------------------------------
void writeCtrlCOM(BYTE* buffer, BOOL calibrationDisable, BOOL regWriteEnable, BOOL programEnable ){
	BYTE writeCtrl[2] = {0x18, 0x04*calibrationDisable+ 0x02*regWriteEnable + 0x01*programEnable};
	mergeBuffer(buffer, writeCtrl, 2);
}


//-------------------------------------------------------------------------------------------------------
// readCtrl - command to send the Control register to the Bridge Buffer
//-------------------------------------------------------------------------------------------------------
void readCtrlCOM(BYTE* buffer){
	BYTE readCtrl[2] = {0x1C, 0x00};
	mergeBuffer(buffer, readCtrl, 2);
}


//-------------------------------------------------------------------------------------------------------
// shutdown - command to set the RDAC in open circuit
//-------------------------------------------------------------------------------------------------------
void shutdownCOM(BYTE* buffer, BOOL shutdownModeOn){
	BYTE shutdown[2] = {0x20, 0x00 + 0x01*shutdownModeOn};
	mergeBuffer(buffer, shutdown, 2);
}


//-------------------------------------------------------------------------------------------------------
// highImp - command to set SDO pin in high impedance for minimum power dissipation
//-------------------------------------------------------------------------------------------------------
void highImpCOM(BYTE* buffer){
	BYTE highImp[2] = {0x80, 0x01};
	mergeBuffer(buffer, highImp, 2);
}
