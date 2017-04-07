/*
 * ConfigINFC.cpp
 *
 *	Console Interface used to configure various aspects of PSUControl.
 *
 *  Created on: 20/06/2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Interface/Interface.h"


//==============================================VARIABLES==============================================//
// Keyboard
extern char categorySelectionINFC;
extern BYTE bufferINFC[I2C_MAX_BUF_SIZE];
char configSelectionINFC ='\0';
char configChar;					// used for WAIT_FOR_KEYBOARD macro, and as auxiliary variable in certain functions
#define WAIT_FOR_KEYBOARD configChar = sgetchar(0);

// SnI config
extern BOOL config_AlarmCTRLUpdate_SnI_FLAG;	// Includes/excludes Supply and Internal voltages in alarmTask
extern BOOL config_MonitorCTRL_SnI_FLAG;		// Includes/excludes SnI voltages in monitorTask
extern BOOL consoleOutputI2C;					// Toggles console messages for I2C and SPI functions
extern BOOL consoleOutputRDAC;					// Toggles console messages for RDAC functions
extern BOOL testMode_MonitorCTRL_Print_FLAG;	// Prints value when valid


//=====================================================================================================//
//====================================== PUBLIC CONFIG MENU ===========================================//
//=====================================================================================================//

void ConfigINFC ( void ){
	ERASE_CONSOLE
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("--------------------------------- CONFIG MENU ---------------------------------\r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf(" (1) Configure Alarm Update period\n");
	iprintf(" (2) Include/Exclude SnIs in alarm and monitor tasks\n");
	iprintf(" (3) Simulate measures\n");
	iprintf(" (4) Toggle RDACLibrary console output\n");
	iprintf(" (5) Toggle I2CnSPILibrary console output\n");
	iprintf(" (6) Set I2C wait period\n");
	iprintf(" (7) Toggle Monitor console output\n");
	iprintf("\n (e) EXIT TO GENERAL MENU \r\n" );
	iprintf("-------------------------------------------------------------------------------\r\n" );
	iprintf("\r\nEnter command " );
	categorySelectionINFC = sgetchar( 0 ); iprintf("%c\n", categorySelectionINFC);
	iprintf("\r\n-------------------------------------------------------------------------------\r\n" );
	processCommandConfigINFC();
}



//=====================================================================================================//
//====================================== PRIVATE CONFIG METHODS =======================================//
//=====================================================================================================//

// Choose Config
void processCommandConfigINFC ( void ){
	switch (categorySelectionINFC){
		case '1': AlmPeriodConfigINFC(); break;
		case '2': snisConfigINFC(); break;
		case '3': simulationINFC(); break;
		case '4': toggleRDACOutputINFC(); break;
		case '5': toggleI2CnSPIOutputINFC(); break;
		case '6': setI2CwaitPeriod(); break;
		case EXIT: iprintf("Exiting to INTERFACE MENU\n");  break;
		default:
				iprintf( "\nINVALID KEY -> %c\r\n", categorySelectionINFC);
				iprintf( " \r\nPRESS ANY KEY TO RETURN TO CONFIG MENU\r\n" );
				break;
	}
	WAIT_FOR_KEYBOARD
}

// (1)
void AlmPeriodConfigINFC ( void ){
	char almPeriodBuf[4];
	iprintf(" Enter the period to be programmed (x50ms):  ");
	int almPeriod = atoi(gets((char *)almPeriodBuf));iprintf("\r\n");
	if(almPeriod>50){iprintf(" Number too big. Setting 50 instead"); almPeriod = 50;}
	if(almPeriod<1){iprintf(" Number too small. Setting 1 instead"); almPeriod = 1;}
	config_alarmUpdatePeriod_x50MS(almPeriod);
}

// (2)
void snisConfigINFC( void ){
	char ch;
	iprintf(" SnI: %s in alarmTask and %s in monitorTask\n", (config_AlarmCTRLUpdate_SnI_FLAG?"enabled":"disabled"), (config_MonitorCTRL_SnI_FLAG?"enabled":"disabled"));
	iprintf(" (1) Toggle SnIs in alarmTask\n ");
	iprintf(" (2) Toggle SnIs in monitorTask\n ");
	iprintf(" Enter the choice: ");
	ch = sgetchar(0); iprintf ("%c\n", ch);
	switch(ch){
		case '1':
			config_AlarmCTRLUpdate_SnI_FLAG = !config_AlarmCTRLUpdate_SnI_FLAG;
			iprintf(" SnIs in alarmTask %s\n ", (config_AlarmCTRLUpdate_SnI_FLAG?"enabled":"disabled"));
			break;

		case '2':
			config_MonitorCTRL_SnI_FLAG = !config_MonitorCTRL_SnI_FLAG;
			iprintf(" SnIs in monitorTask %s\n ", (config_MonitorCTRL_SnI_FLAG?"enabled":"disabled"));
			break;
	}
}

// (3)
void simulationINFC ( void ){
	// TODO: Simulation left for future implementation
}

// (4)
void toggleRDACOutputINFC ( void ){
	config_consoleOutput_RDAC(!consoleOutputRDAC);
}

// (5)
void toggleI2CnSPIOutputINFC ( void ){
	config_consoleOutput_I2CnSPI(!consoleOutputI2C);
}

// (6)
void setI2CwaitPeriod ( void ){
	iprintf(" Enter the period to be programmed (in ms):  ");
	*bufferINFC = 0x00000000;
	config_i2cWait_I2CnSPI (atoi(gets((char*)bufferINFC))*33000 /*Fsys = 33MHz*/);
}

// (5)
void toggleMonitorCTRLPrintFLAG ( void ){
	testMode_MonitorCTRL_Print_FLAG=!testMode_MonitorCTRL_Print_FLAG;
}
