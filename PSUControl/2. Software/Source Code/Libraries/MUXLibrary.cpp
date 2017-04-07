/*
 * MUXLibrary.cpp
 *
 *	Selects an output for all the multiplexers using the configured GPIO pins.
 *	There are 2 steps:
 *		- Selecting the desired output for muxes A to E (different inputs for each)
 *		- Selecting the desired output for mux ADC (inputs are the outputs of muxes A to E and a single value)
 *	For further inquiries please refer to the self-explanatory MUXDiagram.jpg in PSUControl
 *
 *  Created on: 27-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "MUXLibrary.h"


//===========================================VARIABLES=================================================//
// MUX Configuration
BYTE samplingFunctionMUX;		// FUNCTION_PSU_VOLTAGE/FUNCTION_PSU_CURRENT/FUNCTION_SnI_VOLTAGE
BYTE NumMUX;					// 0 to 11 for PSUs, 0 to 12 for SnIs
float scaleFactorMUX=0;			// factor to be multiplied to the read value in ADC to obtain real value
BYTE muxABCDE = 0;				// Addressed value to muxes A to E
BYTE muxADC = 0;				// Addressed value to mux ADC


// Arrays containing the config values for each input
extern float scaleFactorArray[37];
BYTE posMuxABCDE[37] = POS_MUX_ABCDE_ARRAY;
BYTE posMuxADC[37]   = POS_MUX_ADC_ARRAY;

// Simple access to arrays defined in MUXLibrary.h
//	SCALE_FACTOR(sf, num)
//	POSITION_MUX_ABCDE(sf, num)
//	POSITION_MUX_ADC  (sf, num)



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
	A0_MUX_ABCDE.function(PIN28_GPIO); //A0_MUX_ABCDE
	A1_MUX_ABCDE.function(PIN33_GPIO); //A1_MUX_ABCDE
	A2_MUX_ABCDE.function(PIN34_GPIO); //A2_MUX_ABCDE
	EN_MUX_ABCDE.function(PIN8_GPIO);  //EN_MUX_ABCDE
	// MUX_ADC
	A0_MUX_ADC.function(PIN15_GPIO); //A0_MUX_ADC
	A1_MUX_ADC.function(PIN16_GPIO); //A1_MUX_ADC
	A2_MUX_ADC.function(PIN17_GPIO); //A2_MUX_ADC
	EN_MUX_ADC.function(PIN18_GPIO); //EN_MUX_ADC
	// Enable always 1
	EN_MUX_ABCDE = 1; 			//EN_MUX_ABCDE
	EN_MUX_ADC = 1; 			//EN_MUX_ADC
	// Initial Status
	setMUX(FUNCTION_PSU_VOLTAGE, SF1_A); // Sets the initial MUX configuration to Voltage in first PSU
}


//-------------------------------------------------------------------------------------------------------
// setMUX - Configures the Muxes for a PSU/SnI number and the desired function:
//				(0) FUNCTION_PSU_VOLTAGE: Allows PSU Voltage readings.
//				(1) FUNCTION_PSU_CURRENT: Allows PSU current Readings.
//				(2) FUNCTION_SnI_VOLTAGE: Allows Supply and Internal (SnI) voltage readings
// 		#INPUT: BYTE samplingFunction - One of the three options above
//				BYTE psuSniNum - selected PSU/SnI number
// 		#OUTPUT: float scaleFactorMUX - scale value for the selected voltage
//		>E.G.: setMUX( FUNCTION_PSU_VOLTAGE, SF3_A ); - used in MonitorCTRL.cpp
//-------------------------------------------------------------------------------------------------------
float setMUX(BYTE samplingFunction, BYTE psuSniNum){
	// Saving configuration
	samplingFunctionMUX = samplingFunction;
	NumMUX = psuSniNum;
	scaleFactorMUX = SCALE_FACTOR(samplingFunction, psuSniNum);
	muxABCDE = POSITION_MUX_ABCDE(samplingFunction, psuSniNum);
	muxADC = POSITION_MUX_ADC(samplingFunction, psuSniNum);
	//   -TIP: SHUNTs for SF2_A and SF2_B are not connected (design issue in backplane) - Ground is measured instead

		// Setting muxes' addressing pins
		A0_MUX_ABCDE = muxABCDE & 0x1; 	//A0_MUX_ABCDE
		A1_MUX_ABCDE = muxABCDE & 0x2; 	//A1_MUX_ABCDE
		A2_MUX_ABCDE = muxABCDE & 0x4; 	//A2_MUX_ABCDE
		EN_MUX_ABCDE = 1; 				//EN_MUX_ABCDE

		A0_MUX_ADC = muxADC & 0x1; 	//A0_MUX_ADC
		A1_MUX_ADC = muxADC & 0x2; 	//A1_MUX_ADC
		A2_MUX_ADC = muxADC & 0x4; 	//A2_MUX_ADC
		EN_MUX_ADC = 1; 			//EN_MUX_ADC

	return scaleFactorMUX;
}


//-------------------------------------------------------------------------------------------------------
// Getters
//-------------------------------------------------------------------------------------------------------
BYTE getSamplingFunctionMUX ( void ){	return samplingFunctionMUX;	}
BYTE getNumMUX ( void )				{	return NumMUX;				}
BYTE getScaleFactorMUX ( void )		{	return scaleFactorMUX;		}


