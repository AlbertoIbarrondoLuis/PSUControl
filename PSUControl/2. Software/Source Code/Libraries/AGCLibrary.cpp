/*
 * AGCLibrary.cpp
 *
 *	Automatic Gain Control based on a RDAC with an O.A., where the gain value is:
 *
 *				1024 - RDAC Counts
 *		G = 1 + ------------------ * 100
 *					  1024
 *
 *		RDAC_MIN_VALUE = 0    -> Gmax = 101
 *		RDAC_MAX_VALUE = 1023 -> Gmin = 1.1
 *
 *  Created on: 17-abr-2015
 *      Author: Alberto Ibarrondo Luis
 */

#include "AGCLibrary.h"

//===========================================VARIABLES=================================================//
// AGC Main Values
float gainAGC = 1.1;					// Multiplying value for the ADC measure
int rdacCountAGC = 0;					// Value of the RDAC Value Register

// I2C Communication
BYTE resultI2C_AGC = 0;

// Limit booleans
BOOL maxAGCReached = false;
BOOL minAGCReached = false;

// Temporary
float auxGainAGC; int auxRdacCountAGC; BOOL auxMaxAGCReached; BOOL auxMinAGCReached;


//=====================================================================================================//
//======================================= PUBLIC AGC METHODS ==========================================//
//=====================================================================================================//

//-------------------------------------------------------------------------------------------------------
// minAGC - Sets AGC Gain to minimum by configuring the RDAC to minimum value ( 1023 )
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: minAGC() - AGCLibraryINFC.cpp
//-------------------------------------------------------------------------------------------------------
void minAGC( void ){
	auxRdacCountAGC = RDAC_MAX_VALUE;
	setValRDAC(auxRdacCountAGC, AGC_SPI_ADDRESS, AGC_I2C_ADDRESS);
	resultI2C_AGC = getI2CResultRDAC();
	if(resultI2C_AGC==I2C_OK){					// Refreshes Gain and Counts only when I2C_OK
		rdacCountAGC = auxRdacCountAGC;
		gainAGC = countsToGainAGC ( rdacCountAGC );
		maxAGCReached = false;
		minAGCReached = true;
	}
	else{
		// ERROR
	}
}


//-------------------------------------------------------------------------------------------------------
// maxAGC - Sets AGC Gain to maximum by configuring the RDAC to maximum value ( 0 )
// 		#INPUT: None
// 		#OUTPUT: None
//		>E.G.: maxAGC()
//-------------------------------------------------------------------------------------------------------
void maxAGC( void ){
	auxRdacCountAGC = RDAC_MIN_VALUE;
	setValRDAC(auxRdacCountAGC, AGC_SPI_ADDRESS, AGC_I2C_ADDRESS);
	resultI2C_AGC = getI2CResultRDAC();
	if(resultI2C_AGC==I2C_OK){					// Refreshes Gain and Counts only when I2C_OK
		rdacCountAGC = auxRdacCountAGC;
		gainAGC = countsToGainAGC ( rdacCountAGC );
		maxAGCReached = true;
		minAGCReached = false;
	}
	else{
		// ERROR
	}
}


//-------------------------------------------------------------------------------------------------------
// setGainAGC - Sets AGC Gain to the selected value, checking for limits
// 		#INPUT: float gain - Value to be set
// 		#OUTPUT: None
//		>E.G.: setGainAGC(50.5)
//-------------------------------------------------------------------------------------------------------
void setGainAGC( float gain ){
	auxRdacCountAGC = gainToCountsAGC(gain);
	if (auxRdacCountAGC >= RDAC_MAX_VALUE){			auxRdacCountAGC  = RDAC_MAX_VALUE; // Higher than MAX - Set to MAX
		auxGainAGC = countsToGainAGC(auxRdacCountAGC);	auxMinAGCReached = true;	}
	else{												auxMinAGCReached = false;	}
	if (auxRdacCountAGC <= RDAC_MIN_VALUE){			auxRdacCountAGC  = RDAC_MIN_VALUE; // Lower than MIN - Set to MIN
		auxGainAGC = countsToGainAGC(auxRdacCountAGC);	auxMaxAGCReached = true;	}
	else{												auxMaxAGCReached = false;	}
	setValRDAC(auxRdacCountAGC, AGC_SPI_ADDRESS, AGC_I2C_ADDRESS);
	resultI2C_AGC = getI2CResultRDAC();
	if(resultI2C_AGC==I2C_OK){					// Refreshes Gain and Counts only when I2C_OK
		rdacCountAGC = auxRdacCountAGC;
		gainAGC = countsToGainAGC(auxRdacCountAGC);
		maxAGCReached = auxMaxAGCReached;
		minAGCReached = auxMinAGCReached;
	}
	else{
		// ERROR
	}
}


//-------------------------------------------------------------------------------------------------------
// scaleGainAGC - Scales AGC Gain with the selected value -> gainAGC = gainAGC * scaleFactorMUX;
// 		#INPUT: float scaleFactorMUX - Value to multiply the AGC gain.
// 		#OUTPUT: None
//		>E.G.: scaleGainAGC(10)
//-------------------------------------------------------------------------------------------------------
void scaleGainAGC( float scaleFactor ){
	auxGainAGC = gainAGC * scaleFactor;
	auxRdacCountAGC = gainToCountsAGC(auxGainAGC);

	if (auxRdacCountAGC >= RDAC_MAX_VALUE){			auxRdacCountAGC  = RDAC_MAX_VALUE; // Higher than MAX - Set to MAX
		auxGainAGC = countsToGainAGC(auxRdacCountAGC);	auxMinAGCReached = true;	}
	else{												auxMinAGCReached = false;	}
	if (auxRdacCountAGC <= RDAC_MIN_VALUE){			auxRdacCountAGC  = RDAC_MIN_VALUE; // Lower than MIN - Set to MIN
		auxGainAGC = countsToGainAGC(auxRdacCountAGC);	auxMaxAGCReached = true;	}
	else{												auxMaxAGCReached = false;	}

	setValRDAC(auxRdacCountAGC, AGC_SPI_ADDRESS, AGC_I2C_ADDRESS);
	resultI2C_AGC = getI2CResultRDAC();
	if(resultI2C_AGC==I2C_OK){
		rdacCountAGC = auxRdacCountAGC;
		gainAGC = countsToGainAGC(auxRdacCountAGC);
		maxAGCReached = auxMaxAGCReached;
		minAGCReached = auxMinAGCReached;
	}
	else{
		// ERROR
	}
}


//-------------------------------------------------------------------------------------------------------
// readValuesAGC - Read RDAC Value Register using RDACLibrary, and update RDAC counts and gain with the
//					received value
// 		#INPUT: float gain
// 		#OUTPUT: int rdacCount
//		>E.G.: readValuesAGC()
//-------------------------------------------------------------------------------------------------------
float readValuesAGC ( void ){
	auxRdacCountAGC = getValRDAC(AGC_SPI_ADDRESS, AGC_I2C_ADDRESS);
	if(getI2CResultRDAC()==I2C_OK){
		if (auxRdacCountAGC >= RDAC_MAX_VALUE){	minAGCReached = true;}	else{	minAGCReached = false;	}
		if (auxRdacCountAGC <= RDAC_MIN_VALUE){	maxAGCReached = true;}	else{	maxAGCReached = false;	}
		rdacCountAGC = auxRdacCountAGC;
		gainAGC = countsToGainAGC(rdacCountAGC);
	}
	return gainAGC;
}


//-------------------------------------------------------------------------------------------------------
// countsToGainAGC - Conversion from RDAC counts to gain
// 		#INPUT: int rdacCount
// 		#OUTPUT: float gain
//		>E.G.: countsToGainAGC(600)
//-------------------------------------------------------------------------------------------------------
float countsToGainAGC ( int rdacCount ){
	auxGainAGC =  1 + ((1024-(float)rdacCount)/1024)*100;
	return auxGainAGC;
}


//-------------------------------------------------------------------------------------------------------
// gainToCountsAGC - Conversion from gain to RDAC counts
// 		#INPUT: float gain
// 		#OUTPUT: int rdacCount
//		>E.G.: gainToCountsAGC(45.5)
//-------------------------------------------------------------------------------------------------------
int gainToCountsAGC (float gain){
	auxRdacCountAGC = (int) round(1024-(gain-1)/100*1024);
	return auxRdacCountAGC;
}


//-------------------------------------------------------------------------------------------------------
// getter methods for AGC attributes
//-------------------------------------------------------------------------------------------------------
int getCountsAGC 		( void ){	return rdacCountAGC;	}
float getGainAGC 		( void ){	return gainAGC;			}
BYTE getI2CResultAGC 	( void ){	return resultI2C_AGC;	}
BOOL getMaxReachedAGC 	( void ){ 	return maxAGCReached;	}
BOOL getMinReachedAGC 	( void ){ 	return minAGCReached;	}
