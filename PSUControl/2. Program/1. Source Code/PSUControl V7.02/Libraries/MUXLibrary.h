/*
 * MUXLibrary.h
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

#ifndef LIB_MUX
#define LIB_MUX

#include "Headers.h"
#include "Libraries/I2C&SPILibrary.h"	// I2C&SPI Communication



//=====================================MUX METHODS===========================================//
//*********Public Methods*********//
void initMUX ( void );
float setMUX(BYTE samplingFunction, BYTE psuSniNum);
BYTE getSamplingFunctionMUX ( void );
BYTE getNumMUX ( void );

//*********Private Methods*********//
void setPinsMUX ( BYTE inputNum, BYTE Mux);


#endif /* MUXLIBRARY_H_ */
