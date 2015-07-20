/*
 * Interface.h
 *
 *	Console based interface used to program the system, display its status and configuration and use all the
 *	Library methods. It also calls TestMenu, allowing the use of testing functions.
 *
 *  Created on: 24/05/2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "Headers.h"
#include "Controller/Controller.h"				// All the Controller Methods
#include "Libraries/Libraries.h"				// All the Libraries' Methods
#include "Tests/Tests.h"						// Testing
#include "Interface/LibrariesInterface/LibrariesInterface.h"	// Interface for Libraries' Methods


//=====================================GENERAL Interface======================================//
//*********Public Methods*********//
void InferfaceMain ( void ); 			// Call to access all the code via terminal. Can be exited

//*********Private Methods********//
void InterfaceMenuINFC ( void );		// INTERFACE MENU.


//==========================================MODULES===========================================//
//------------------------------------ CONFIG Interface --------------------------------------//
//*********Public Methods*********//
void ConfigINFC ( void );							// CONFIG MENU

//*********Private Methods********//
void processCommandConfigINFC ( void );				// Choose Config
// Config Methods
void AlmPeriodConfigINFC ( void );					// (1)
void snisConfigINFC( void );						// (2)
void simulationINFC ( void );						// (3)
void toggleRDACOutputINFC ( void );					// (4)
void toggleI2CnSPIOutputINFC ( void );				// (5)


//---------------------------------- DISPLAY Interface ---------------------------------------//
//*********Public Methods*********//
void DisplayINFC ( void );							// DISPLAY MENU

//*********Private Methods********//
void processCommandDisplayINFC ( void );			// Choose Display
// Status
void statusDisplayINFC ( void );					// (1)
void refreshAlarmStatusINFC ( void );
void refreshValuesStatusINFC ( void );
// Configuration
void configDisplayINFC ( BOOL psu_sni );			// (2)
void refreshAlarmConfigINFC ( BOOL psu_sni );
void refreshValuesConfigINFC ( BOOL psu_sni );


//---------------------------------- LIBRARIES' Interface -------------------------------------//
//*********Public Methods*********//
void LibrariesINFC ( void );							// LIBRARIES MENU
// Libraries' Methods, all accessible in folder Interface/LibrariesInterface
void I2CnSPILibraryINFC( void );					// (1) - I2C & CPI Library
void RDACLibraryINFC( void );						// (2) - Digital potentiometers' Library
void AGCLibraryINFC ( void );						// (3) - Automatic Gain Control Library
void RelayFunctionsDISP ( void );					// (4) - Relays' Library
void MUXLibraryINFC ( void );						// (5) - Multiplexers' Library

//*********Private Methods********//
void processCommandLibraryINFC ( void );			// Choose Library


//--------------------------------- PROGRAMMING Interface ------------------------------------//
//*********Public Methods*********//
void ProgramINFC ( void );							// PROGRAM MENU

//*********Private Methods********//
void processCommandProgramINFC ( void );			// Choose Program
void alarmProgramMenuINFC ( BOOL psu_sni );			// (4/6) ALARM MENU (AlmMenu)
// Selectors
void selectNumsINFC ( BOOL psu_sni );				// Select PSUs/SnIs
void selectAlarmsINFC ( BOOL psu_sni );				// Select Alarms
void selectProtocolsINFC ( BOOL psu_sni );			// Select Alarm Protocols
void selectShutdownINFC ( BOOL psu_sni );			// Select PSUs for Protocol Shutdown
// Programming methods
void switchOnOffINFC ( void );						// (1)
void changeOutputVoltageINFC ( void );				// (2)
void programInitializationTimerINFC ( void );		// (5)
void programAlarmLimitTimesINFC ( BOOL psu_sni );	// AlmMenu - (1)
void programAlarmLimitValuesINFC ( BOOL psu_sni );	// AlmMenu - (2)
void programAlarmWatchINFC ( BOOL psu_sni );		// (3/7)
void programAlarmProtocolsINFC ( BOOL psu_sni );	// AlmMenu - (3)
void programAlarmProtocolShutdownINFC(BOOL psu_sni);// AlmMenu - (4)
void programAlarmProtocolModVoltINFC ( void );		// AlmMenu - (5)
void programDefaultValuesINFC(BOOL psu_sni);		// (+/-)



#endif /* INTERFACE_H_ */
