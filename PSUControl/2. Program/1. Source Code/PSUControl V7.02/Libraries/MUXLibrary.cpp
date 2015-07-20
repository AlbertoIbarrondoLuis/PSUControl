/*
 * MUXLibrary.cpp
 *
 *	Selects an output for all the multiplexers using the configured GPIO pins.
 *	There are 2 steps:
 *		- Selecting the desired output for muxes A to E (different inputs for each)
 *		- Selecting the desired output fof mux ADC (inputs are the outputs of muxes A to E and a single value)
 *	For further inquiries please refer to the self-explanatory MUXDiagram.jpg in PSUControl
 *
 *  Created on: 27-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "MUXLibrary.h"


//===========================================VARIABLES=================================================//
// MUX Configuration
BYTE samplingFunctionMUX;		// FUNCTION_PSU_VOLTAGE/FUNCTION_PSU_CURRENT/FUNCTION_SnI_VOLTAGE
BYTE NumMUX;					// 0 to 11 for PSUs, 0 to 13 for SnIs

// Scale Factor for ADC readings, accessible with _sf(samplingFunction, Num)
extern float scaleFactorArray[38];
float scaleFactor=0;


//=====================================================================================================//
//====================================   PUBLIC MUX METHODS    ========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// initMUX - Initializes all the Pins used for Mux control
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: initMUX(); - used in main.cpp
//-------------------------------------------------------------------------------------------------------
void initMUX ( void ){
	// MUX_ABCDE
	Pins[15].function(PIN15_GPIO); //A0_MUX_ABCDE
	Pins[16].function(PIN16_GPIO); //A1_MUX_ABCDE
	Pins[17].function(PIN17_GPIO); //A2_MUX_ABCDE
	Pins[18].function(PIN18_GPIO); //EN_MUX_ABCDE
	// MUX_ADC
	Pins[28].function(PIN28_GPIO); //A0_MUX_ADC
	Pins[33].function(PIN33_GPIO); //A1_MUX_ADC
	Pins[34].function(PIN34_GPIO); //A2_MUX_ADC
	Pins[35].function(PIN35_GPIO); //EN_MUX_ADC
	// Initial Status
	setMUX(FUNCTION_PSU_VOLTAGE, SF1_A); // Sets the initial MUX configuration to Voltage in first PSU
}


//-------------------------------------------------------------------------------------------------------
// setMUX - Configures the Muxes for a PSU/SnI number and the desired function:
//				(0) FUNCTION_PSU_VOLTAGE: Allows PSU Voltage readings.
//				(1) FUNCTION_PSU_CURRENT: Allows PSU current Readings.
//				(2) FUNCTION_SnI_VOLTAGE: Allows Supply and Internal (SnI) voltage readings
// 		#INPUT: BYTE samplingFunction - One of the three options above
//				BYTE Num - selected PSU/SnI number
// 		#OUTPUT: float scaleFactor - scale value for the selected voltage
//		>E.G.: setMUX( FUNCTION_PSU_VOLTAGE, FUNCTION_PSU_CURRENT ); - used in MonitorCTRL.cpp
//-------------------------------------------------------------------------------------------------------
float setMUX(BYTE samplingFunction, BYTE psuSniNum){
	samplingFunctionMUX = samplingFunction; NumMUX = psuSniNum;
	scaleFactor = scaleFactorArray[_sf(samplingFunction, psuSniNum)] * DEFAULT_SCALE_FACTOR;
	switch(samplingFunction){

		case FUNCTION_PSU_VOLTAGE:
			if (psuSniNum > SF4_B){
				if (psuSniNum > SF5_B){	setPinsMUX(psuSniNum-8, MUX_E);}
				else {				setPinsMUX(psuSniNum-8, MUX_B);}}
			else{					setPinsMUX(psuSniNum, MUX_A);}
			break;

		case FUNCTION_PSU_CURRENT:
			if (psuSniNum>SF4_B){ setPinsMUX(psuSniNum-4, MUX_B);}
			else{			setPinsMUX(psuSniNum, MUX_C);}
			break;

		case FUNCTION_SnI_VOLTAGE:
			switch (psuSniNum){
				case SUP_n16_REG: case SUP_n20_UNREG: 	setPinsMUX(psuSniNum-8, MUX_E); break;
				case SUP_12V_F_D: 						setPinsMUX(0, 5); break;
				case INT_VCC_3V3: case INT_VCC_12V: 	setPinsMUX(psuSniNum-9, MUX_B); break;
				case INT_VCC_n12V: 						setPinsMUX(psuSniNum-9, MUX_E); break;
				default: 								setPinsMUX(psuSniNum, MUX_D); break;
			}
			break;

		default: iprintf(" ~ERROR MUX: Unsupported Function\n");	break;
	}
	return scaleFactor;
}


//-------------------------------------------------------------------------------------------------------
// Getters
//-------------------------------------------------------------------------------------------------------
BYTE getSamplingFunctionMUX ( void ){	return samplingFunctionMUX;	}
BYTE getNumMUX ( void )				{	return NumMUX;				}






//=====================================================================================================//
//====================================  PRIVATE MUX METHODS  ==========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// setPinsMUX - Sets all the GPIO pins to select MUX a to E and MUX_ADC outputs
// 		#INPUT: BYTE Num - 0 to 7 - Input number for muxes A to E that will be connected to their outputs
//				BYTE Mux - MUX_A/MUX_B/MUX_C/MUX_D/MUX_E/5 to 7 - Input number for mux ADC that will be
//							connected to its output, selecting the output of one of the muxes A to E
// 		#OUTPUT: none
//		>E.G.: setMUX(Num-9, MUX_E ); - used in MonitorCTRL.cpp
//-------------------------------------------------------------------------------------------------------
void setPinsMUX ( BYTE inputNum, BYTE Mux){

	A0_MUX_ABCDE = inputNum & 0x1; 	//A0_MUX_ABCDE
	A1_MUX_ABCDE = inputNum & 0x2; 	//A1_MUX_ABCDE
	A2_MUX_ABCDE = inputNum & 0x4; 	//A2_MUX_ABCDE
	EN_MUX_ABCDE = 1; 			//EN_MUX_ABCDE

	A0_MUX_ADC = inputNum & 0x1; 	//A0_MUX_ADC
	A1_MUX_ADC = inputNum & 0x2; 	//A1_MUX_ADC
	A2_MUX_ADC = inputNum & 0x4; 	//A2_MUX_ADC
	EN_MUX_ADC = 1; 			//EN_MUX_ADC
}



