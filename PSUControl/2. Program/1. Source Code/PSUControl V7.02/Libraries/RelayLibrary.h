/*
 * RelayLibrary.h
 *
 *	Connects/Disconnects relays for all 12 PSUs. PSUs 0 to 5 (SF1_A to SF3_B) are controlled by
 *	GPIO pins, while PSUs 6 to 11 (SF1_A to SF3_B) are switched using an I2C Bus expander
 *
 *  Created on: 27-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef LIB_RELAY
#define LIB_RELAY

#include "Headers.h"
#include "Libraries/I2C&SPILibrary.h"	// I2C&SPI Communication


//====================================RELAY METHODS==========================================//
//*********Public Methods*********//
// Relay connection/disconnection
void connectRelay ( int psuNum );
void disconnectRelay ( int psuNum );
void connectSeveralRelay ( BOOL selectedPSUs[PSU_NUMBER] );
void disconnectSeveralRelay ( BOOL selectedPSUs[PSU_NUMBER] );
// Getters
BOOL getStatusRelay ( int psuNum );
BYTE getI2CResultRelay ( void );

#endif /* LIB_RELAY */
