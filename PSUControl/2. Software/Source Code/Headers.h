/*
 * Headers.h
 *
 *	Contains all the external files required for this system, well-organized and explained
 *
 *  Created on: 09-abr-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef HEADERS_H_
#define HEADERS_H_

#include "predef.h"
#include <stdio.h>
#include <basictypes.h>					// Include for user-defined types (BOOL & BYTE)

#include <a2d.h>						// Analog to digital converter
#include <bsp.h>            			// MOD5213 board support package interface
#include <constants.h>     				// Include for constants like MAIN_PRIO
#include <ctype.h>
#include <gdbstub.h>
#include <i2cmulti.h>            		// Used for Multi-Master I2C
#include <Pins.h>						// NetBurner Pins Class
#include <Math.h>						// Methods Ceil() and Floor()
#include <serialirq.h>					// Use serial interrupt driver
#include <serialupdate.h>				// Update flash via serial port
#include <sim5213.h>        			// Access to MCF5213 register structures
#include <smarttrap.h>					// NetBurner Smart Trap utility
#include <stdlib.h>
#include <string.h>
#include <system.h>						// Include for system functions
#include <ucos.h>						// Include for RTOS functions
#include <ucosmcfc.h>
#include <utils.h>          			// Include for usage of carrier board LEDs
#include <cfinter.h>

#include "defineConstants.cpp"			// Constants everywhere, contains default values

#include "Typedefs/PSU_TYPE.cpp"		// Declaration of the types PSU_TYPE and SnI_TYPE
#include "Typedefs/SnI_TYPE.cpp"


//======================================INTERRUPTION METHODS=============================================//
void SetUpPITR(WORD clock_interval, BYTE pcsr_pre /* See table 17-3 in the users manual for bits 8-11*/);
void SetUpIRQ1( void );

//======================================INTERRUPTION COUNTERS============================================//
extern DWORD pitr_count_zero;
extern DWORD pitr_count_adc_sampling;

#endif /* HEADERS_H_ */
