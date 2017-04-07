/*
 * GeneralFunctionsINFC.cpp
 *
 *	Not constructed yet: it should contain interface methods for all the GeneralLibrary.
 *	Reason: doen't seem to be useful enough
 *
 *  Created on: 22-jun-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods

//==============================================VARIABLES==============================================//
// Keyboard
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];


//=====================================================================================================//
//==============================   GENERAL FUNCTIONS INTERFACE   ======================================//
//=====================================================================================================//

// Under construction!

int selectPSUNumINFC ( void ){
	int psuNum;
	iprintf( " Select PSU number (0 to 11): " );
	*bufferINFC = 0x0000;
	psuNum=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	while ((psuNum<0) || (psuNum>=PSU_NUMBER)){
		iprintf( " Invalid number. Select a number from 0 to 11: " );
		psuNum=atoi(gets((char*)bufferINFC));iprintf("\r\n");
	}
	return psuNum;
}
