/*
 * Tests.h
 *
 *	Test console interface in mainTEST, which grants access to Controller tests (one for each
 *	controller module), Libraries' tests (one for each Library) and PSUControl tests (involving
 *	the whole system).
 *
 *  Created on: 10-mar-2015
 *      Author: Alberto Ibarrondo Luis
 */

#ifndef TESTS_H_
#define TESTS_H_

#include "Headers.h"
#include "Controller/Controller.h"		// All the Controller Methods
#include "Interface/Interface.h"		// Status display, Program and function menus


//==========================================MODULES===========================================//

//------------------------------------------ Main --------------------------------------------//
//*********Public Methods*********//
void TestMain ( void );						// Call this method to access all the tests via terminal. Can be exited

//*********Private Methods********//
void TestMenu ( void );						// TEST MENU


//--------------------------------Libraries' TEST methods-------------------------------------//
//*********Public Methods*********//
// Menu
void LibrariesTEST ( void );				// LIBRARIES TEST MENU
// Tests
BOOL TEST_I2CSPILibrary ( void );			// (1)
BOOL TEST_RDACLibrary ( void );				// (2)
BOOL TEST_AGCLibrary( void );				// (3)
BOOL TEST_MUXLibrary ( void );				// (4)
BOOL TEST_RelayLibrary ( void );			// (5)

//--------------------------------Controller TEST methods-------------------------------------//
//*********Public Methods*********//
// Menu
void ControllerTEST ( void );				// CONTROLLER TEST MENU
// Global Tests
void TEST_Timer_Interruption ( void );		// (1)
BOOL TEST_AlarmCTRL( void );				// (2)
BOOL TEST_DataListsCTRL ( void );			// (3)
BOOL TEST_FlashMemCTRL( void );				// (4)
BOOL TEST_MonitorCTRL( void );				// (5)
BOOL TEST_SwitchOnCTRL( void );				// (6)
BOOL TEST_VoltCurrCTRL( void );				// (7)

//*********Private Methods********//
// Partial Tests
void TEST_AlarmCTRL_PSU1(void);
void TEST_AlarmCTRL_PSU2(void);
void TEST_AlarmCTRL_PSU3(void);
void TEST_AlarmCTRL_PSU4(void);
void TEST_AlarmCTRL_SnI1(void);
void TEST_AlarmCTRL_SnI2(void);
void TEST_AlarmCTRL_SnI3(void);
void TEST_FlashMemCTRL_PSU (void);
void TEST_FlashMemCTRL_SnI(void);
void TEST_MonitorCTRL_HIGHVALUES( void );
void TEST_MonitorCTRL_NOMINALVALUES( void );



//--------------------------------PSUControl TEST methods-------------------------------------//
//*********Public Methods*********//
void PSUControlTEST ( void );				// PSUControl TEST MENU

#endif /* TESTS_H_ */
