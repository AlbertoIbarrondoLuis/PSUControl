
/**********************************************************************************
 *
 * 				MCF5213 12-PSU Monitoring and Control System - PSUControl
 *
 * 	PSUControl
 *
 * 	 Software system for Netburner MOD5213 which controls the output of twelve
 * 	 independent Power Supply Units (a.k.a. PSUs) modifying their voltage and
 * 	 controlling their relays. It also monitors all the PSUs' outputs and several
 * 	 Supply and Internal Voltages (a.k.a. SnIs) using MUXes and the Netburner A2D
 * 	 module, triggering alarms qhen certain programmable limits are reached.
 *
 * 	 PSUControl is a subsystem of a 300GHz Radar used for detection of objects
 * 	 under the clothes, developed by SSR department in ETSIT - UPM.
 *
 *
 *  main.cpp
 *
 *   Main Task of PSUControl, in charge if initializing the whole system (including
 *   higher priority tasks) and launching the interface (InterfaceMain())
 *
 *   Author: Alberto Ibarrondo Luis
 *
 *********************************************************************************/
float versionNo =7.01;

#include "headers.h"

#include "Libraries/AGCLibrary.h"		// AGC - Automatic Gain Control
#include "Libraries/GeneralLibrary.h"	// Functions with various general purposes
#include "Libraries/I2C&SPILibrary.h"	// I2C&SPI Communication
#include "Libraries/MUXLibrary.h"		// Multiplexers
#include "Libraries/RDACLibrary.h"		// RDAC - Digital potentiometers
#include "Libraries/RelayLibrary.h"		// Relay control

#include "Controller/Controller.h"		// All the Controller Methods
#include "Tests/Tests.h"				// All the testing Methods
#include "Interface/Interface.h"		// Status display, Program and function menus

//========================================DECLARATIONS===============================================//
//------------------------------------------Methods--------------------------------------------------//
extern "C" {
	void UserMain( void * pd);

	// This function sets up the 5213 interrupt controller
	void SetIntc(long func, int vector, int level, int prio);
}
void _init (void);

//-----------------------------------------Variables-------------------------------------------------//
// AppName
const char * AppName="PSUControl";

// I2C Address Array
static BYTE i2CtoSPIAddress[PSU_NUMBER]= PSUs_I2C_ADDRESS_ARRAY;	// Defined by 3 switches.

// Keyboard Stop
char mainChar;
#define WAIT_FOR_KEYBOARD mainChar=sgetchar(0);

// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];			// MAIN PSU ARRAY LIST
// extern SnI_TYPE sniList[INT_VCC_n12V + 1];		// Supply & Internal voltages List



//========================================USERMAIN===================================================//
void UserMain(void * pd) {
	_init();									// Defined below. Contains all the init & configuration issues.

//----------------------------------------Main Loop--------------------------------------------------//
	while (true) { //loop forever
		InferfaceMain();
	}
}





//=====================================System Initializations========================================//
void _init (void){
	int i; DWORD res = 0;
	//-------------------------------------General System--------------------------------------------//
	SimpleUart(0,SystemBaud);assign_stdio(0);				// Serial port 0 for Data
	SimpleUart(1,SystemBaud);								// Serial port 1 for Debug
	EnableSmartTraps();
	#ifdef _DEBUG
		InitGDBStubNoBreak( 1, 115200 );
	#endif
	OSChangePrio(MAIN_PRIO);								//Other
	EnableSerialUpdate();

	iprintf("\n\n\n\n\n\n\n\nINITIALIZATION\n");
	putleds(8);
	iprintf("Version Number --> %s\n", ftos(versionNo, 3));
	createVERIFY_KEY();										// Using versionNo to update Flash DDBB

	//--------------------------------------I2C Bus Connection---------------------------------------//
	I2CInit(MCF5213_I2C_ADDRESS, I2C_FREQUENCY); 			// Initialize I2C with Predefined Address (0x20)
															// Set frequency division to 768 (66Mhz CF clock --> 88,25 Khz I2C bus)
	iprintf(" .Initialized I2C address for MCF5213: 0x%x\r\n",MCF5213_I2C_ADDRESS);
	iprintf("   -Set I2C Frequency division: %x (MCF internal CLK / 1280)\r\n",I2C_FREQUENCY);


	//-------------------------------------SPI Bus Connection----------------------------------------//
	res=true;
	for (i = 0; i<PSU_NUMBER; i++){
		res &=configureSPI( INIT_SPI_ORDER, INIT_SPI_CPOL, INIT_SPI_CPHA, INIT_SPI_CLKRATE, i2CtoSPIAddress[i]);
	}
	if(res==I2C_OK){iprintf(" .SPI bus configured\r\n");}
	else{iprintf(" .SPI configuration ERROR - Coudn't be configured\r\n");}

	//-----------------------------------RDACs with minimum value------------------------------------//

	res = 0;
	for (i = 0; i<PSU_NUMBER; i++){
		setValRDAC(INITIAL_VOLTAGE, (i&0x1?UPPER_SLAVE_SPI_ADDRESS:LOWER_SLAVE_SPI_ADDRESS), i2CtoSPIAddress[i]);
		res |= getI2CResultRDAC();
	}

	if(res==I2C_OK){iprintf(" .RDACs set to minimum value\r\n");}
	else{iprintf(" .RDAC setting ERROR - Couldn't be set to minimum value\r\n");}


	//----------------------------------------DataLists----------------------------------------------//
	initializeValuesPSUsSnIs();				// Load psuList values from RAM or set them to default
	for (i = 0; i<PSU_NUMBER; i++){			// Start all PSU objects in OFF status, regardless of the loaded status
		psuList[i].relayStatus = OFF;
		psuList[i].psuStatus = OFF;
		psuList[i].ReadyToConnect = false;
	}

	//-------------------------------------------GPIO------------------------------------------------//
	RESET_RHEOSTATS.function( PIN21_GPIO ); 	// RDAC Reset
	LED_OUT_ON.function( PIN7_GPIO ); 			// LED OUT
	iprintf(" .Pin 21 (TIN3) set as GPIO for RDAC Reset\r\n");
	iprintf(" .Pin 7 (IRQ4) set as GPIO for LED OUT\r\n");

	//----------------------------------Analog to Digital Converter----------------------------------//
	Pins[13].function( PIN13_AN0 );					// Configure the A2D pin nº13 as analog input
	EnableAD();
	initMUX();										// Initializes all the Pins used for Mux control
	res=configureSPI( INIT_SPI_ORDER, INIT_SPI_CPOL, INIT_SPI_CPHA, INIT_SPI_CLKRATE, AGC_I2C_ADDRESS);
	minAGC();										// Initializes the AGC with minimum gain
	res = (getI2CResultAGC()==I2C_OK);
	if(res){iprintf(" .ADC initialized\r\n");}
	else{iprintf(" .ADC initialization ERROR\r\n");}


	//--------------------------------------Button Interruption--------------------------------------//
	SetUpIRQ1();
	iprintf(" .Button Interruption Set\r\n");


	//---------------------------------------TimerInterruptions--------------------------------------//
	//SetUpPITR(0, 8294, 1); //Use PITR 0, Wait 8294 clocks, Divide by 2 from table 17-3, 2KHz - NOT IN USE, messes with RTOS Tasks. Best left unused
	SetUpPITR(1, 16588, 1); // Use PITR 1, Wait 16588 clocks, Divide by 2 from table 17-3, 1KHz - Interrupt in charge of A2D sampling
	iprintf(" .Timers initialized\r\n");

    //----------------------------------------- RTOS Tasks-------------------------------------------//
	OSSimpleTaskCreate( alarmTask, ALARM_PRIO );
	iprintf(" .alarmTask initialized\r\n");
	iprintf(" .MonitorSem %s\n", (allSemInit()==OS_NO_ERR?"Initialized CORRECTLY":"Initialization ERROR"));
	OSSimpleTaskCreate( monitorTask, MONITOR_PRIO );
	iprintf(" .monitorTask initialized\r\n");



	iprintf("APPLICATION INITIALIZED\n\nPress one key to begin \n\n");
	WAIT_FOR_KEYBOARD
	putleds(0);
}

