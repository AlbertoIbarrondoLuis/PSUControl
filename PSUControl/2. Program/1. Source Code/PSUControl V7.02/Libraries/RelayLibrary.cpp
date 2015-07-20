/*
 * RelayLibrary.cpp
 *
 *	Connects/Disconnects relays for all 12 PSUs. PSUs 0 to 5 (SF1_A to SF3_B) are controlled by
 *	GPIO pins, while PSUs 6 to 11 (SF1_A to SF3_B) are switched using an I2C Bus expander
 *
 *  Created on: 27-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "Libraries/RelayLibrary.h"

//==============================================VARIABLES==============================================//
// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];	// Power supply units' array list

// I2C Bus Expander
BYTE expanderBusBuffer[1] = {0};		// I2C Bus expander buffer
BYTE mask;								// Auxiliary BYTE used to modify expanderBusBuffer

// Relays
#define CONNECTED 0x1
#define DISCONNECTED 0x0

// I2C
BYTE resultI2C_Relay = 0;				// I2C communication Results


//=====================================================================================================//
//===================================   PUBLIC RELAY METHODS   ========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// initRelay - Initializes all the Pins used for Relays (SF1_A to SF3_B) as GPIO
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: initRelay(); - used in main.cpp
//-------------------------------------------------------------------------------------------------------
void initRelay ( void ){
	Pins[22].function( PIN22_GPIO ); // Relay SF1_A (0)
	Pins[23].function( PIN23_GPIO ); // Relay SF1_B (1)
	Pins[24].function( PIN24_GPIO ); // Relay SF2_A (2)
	Pins[25].function( PIN25_GPIO ); // Relay SF2_B (3)
	Pins[26].function( PIN26_GPIO ); // Relay SF3_A (4)
	Pins[27].function( PIN27_GPIO ); // Relay SF3_B (5)
}


//-------------------------------------------------------------------------------------------------------
// connectRelay - Connects the Relay for the selected PSU
// 		#INPUT: int psuNum - PSU whose relay will be connected
// 		#OUTPUT: None
//		>E.G.: connectRelay(5); - used in SwitchOnCTRL.cpp
//-------------------------------------------------------------------------------------------------------
void connectRelay ( int psuNum ){
	if (psuNum > SF3_B){
		mask = 1<<(psuNum - 6); 		// SF4_A -> 0
		expanderBusBuffer[0] = ( expanderBusBuffer[0] & !mask ) | ( mask );
		resultI2C_Relay = sendI2CMessage (expanderBusBuffer, 1, EXPANDER_I2C_ADDRESS);
		if(resultI2C_Relay==I2C_OK){
			psuList[psuNum].relayStatus=true;
		}
		else{
			iprintf("	~ERROR - RelayLibrary: Relay %d couldn't be configured\n", psuNum);
		}
	}
	else{
		Pins[psuNum+22]=CONNECTED; 		// SF1_A -> (22)
		psuList[psuNum].relayStatus=true;
	}
}


//-------------------------------------------------------------------------------------------------------
// disconnectRelay - Disconnects the Relay for the selected PSU
// 		#INPUT: int psuNum - PSU whose relay will be disconnected
// 		#OUTPUT: None
//		>E.G.: disconnectRelay(5); - used in SwitchOnCTRL.cpp
//-------------------------------------------------------------------------------------------------------
void disconnectRelay ( int psuNum ){
	if (psuNum > SF3_B){
		mask = 1<<(psuNum - 6); 		// SF4_A -> 0
		expanderBusBuffer[0] = ( expanderBusBuffer[0] & !mask );
		resultI2C_Relay = sendI2CMessage (expanderBusBuffer, 1, EXPANDER_I2C_ADDRESS);
		if(resultI2C_Relay==I2C_OK){
			psuList[psuNum].relayStatus=false;
		}
		else{
			iprintf("	~ERROR - RelayLibrary: Relay %d couldn't be configured\n", psuNum);
		}
	}
	else{
		Pins[psuNum+22]=DISCONNECTED; 	// SF1_A -> (22)
		psuList[psuNum].relayStatus=false;
	}
}


//-------------------------------------------------------------------------------------------------------
// connectSeveralRelay - Connects the Relays for the selected PSUs
// 		#INPUT: BOOL selectedPSUs[PSU_NUMBER] - PSU list whose relays will be connected
// 		#OUTPUT: None
//		>E.G.: connectSeveralRelay(selectedPSUs);
//-------------------------------------------------------------------------------------------------------
void connectSeveralRelay ( BOOL selectedPSUs[PSU_NUMBER] ){
	int i;
	for (i = 0; i<6; i++){	if(selectedPSUs[i]){						// SF1_A to SF3_B
			Pins[i+22]=CONNECTED;
			psuList[i].relayStatus=true;
	}}
	mask=0;
	for (i = 0; i<6; i++){	if(selectedPSUs[i+6]){						// SF4_A to SF6_B
		mask |= 1<<i;
	}}
	expanderBusBuffer[0] = ( expanderBusBuffer[0] & !mask ) | ( mask );	// Sets mask bits to 1
	resultI2C_Relay = sendI2CMessage (expanderBusBuffer, 1, EXPANDER_I2C_ADDRESS);
	if(resultI2C_Relay==I2C_OK){
		for (i = SF4_A; i<=SF6_B; i++){	if(selectedPSUs[i]){
			psuList[i].relayStatus=true;
		}}
	}
}


//-------------------------------------------------------------------------------------------------------
// disconnectSeveralRelay - Disconnects the Relays for the selected PSUs
// 		#INPUT: BOOL selectedPSUs[PSU_NUMBER] - PSU list whose relays will be disconnected
// 		#OUTPUT: None
//		>E.G.: disconnectSeveralRelay(selectedPSUs);
//-------------------------------------------------------------------------------------------------------
void disconnectSeveralRelay ( BOOL selectedPSUs[PSU_NUMBER] ){
	int i;
	for (i = 0; i<6; i++){	if(selectedPSUs[i]){						// SF1_A to SF3_B
			Pins[i+22]=DISCONNECTED;
			psuList[i].relayStatus=DISCONNECTED;
	}}
	mask=0;
	for (i = 0; i<6; i++){	if(selectedPSUs[i+6]){						// SF4_A to SF6_B
		mask |= 1<<i;
	}}
	expanderBusBuffer[0] = ( expanderBusBuffer[0] & !mask );	// Sets mask bits to 1
	resultI2C_Relay = sendI2CMessage (expanderBusBuffer, 1, EXPANDER_I2C_ADDRESS);
	if(resultI2C_Relay==I2C_OK){
		for (i = SF4_A; i<=SF6_B; i++){	if(selectedPSUs[i]){
			psuList[i].relayStatus=DISCONNECTED;
		}}
	}
}


//-------------------------------------------------------------------------------------------------------
// Getters
//-------------------------------------------------------------------------------------------------------
BOOL getStatusRelay ( int psuNum ){		return psuList[psuNum].relayStatus;	}
BYTE getI2CResultRelay ( void ){		return resultI2C_Relay;				}
