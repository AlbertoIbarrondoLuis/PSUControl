/*
 * AGCLibrary.h
 *
 *	Automatic Gain Control based on a RDAC with an O.A., where the gain value is:
 *		G = 1 + RDAC Counts / 10.24
 *		Gmax -> 101
 *		Gmin ->	1.1
 *
 *  Created on: 17-abr-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef LIB_AGC
#define LIB_AGC

#include "Headers.h"
#include "Libraries/RDACLibrary.h"		// RDAC - Digital potentiometers

//=====================================AGC METHODS===========================================//
//*********Public Methods*********//
// Configuration
void minAGC( void );
void maxAGC( void );
void setGainAGC( float num );
void scaleGainAGC( float num );

// Conversion
float countsToGainAGC ( int rdacCount );
int gainToCountsAGC ( float gain );

// Getters
float readValuesAGC ( void );
int getCountsAGC ( void );
float getGainAGC ( void );
BYTE getI2CResultAGC ( void );

#endif /* AGCLIBRARY_H_ */
