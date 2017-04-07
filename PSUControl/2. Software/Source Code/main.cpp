
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
float versionNo =8.0;

#include "headers.h"

#include "Libraries/AGCLibrary.h"		// AGC - Automatic Gain Control
#include "Libraries/GeneralLibrary.h"	// Functions with various general purposes
#include "Libraries/I2C&SPILibrary.h"	// I2C&SPI Communication
#include "Libraries/MUXLibrary.h"		// Multiplexers
#include "Libraries/RDACLibrary.h"		// RDAC - Digital potentiometers
#include "Libraries/RelayLibrary.h"		// Relay control

#include "Controller/Controller.h"		// All the Controller Methods
//#include "Tests/Tests.h"				// All the testing Methods
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
BYTE i2CAddressArray[PSU_NUMBER]= PSUs_I2C_ADDRESS_ARRAY;	// Defined by 3 switches.

// Keyboard Stop
char mainChar;
#define WAIT_FOR_KEYBOARD mainChar=sgetchar(0);

// Data Lists
extern PSU_TYPE psuList[PSU_NUMBER];			// MAIN PSU ARRAY LIST
// extern SnI_TYPE supList[INT_VCC_n12V + 1];		// Supply & Internal voltages List

// Relays
extern BYTE expanderBusBuffer[1];

// Configuration Controller
extern BOOL config_MonitorCTRL_SnI_FLAG;	// Supply & Internal voltages
extern BOOL config_MonitorCTRL_PrintVals;	// Supply & Internal voltages
extern BOOL config_AlarmCTRLUpdate_SnI_FLAG;// Includes/excludes SnI voltage
											// alarms when using updateAlarms(void)
extern int alarmUpdatePeriodx50MS;			// Period x50ms for alarm update task
extern BOOL consoleOutputRDAC;				// Toggles console messages for RDAC functions
extern BOOL consoleOutputI2C;				// Toggles console messages for I2C and SPI functions


//  ---Pause Tasks
extern BOOL idleMode_AlarmCTRL_Task_FLAG;	// Sets Alarm Task into idle mode
extern BOOL seqMode_MonitorCTRL_Task_FLAG;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem

//  ---Testing Booleans
extern BOOL testMode_MonitorCTRL_Measure_FLAG;// Uses testMode_Measure instead of reading from ADC
extern BOOL testMode_SwitchOnCTRL_Task_FLAG;// Stores the PSUs waiting to be switched on in remainingPSUs[20]
extern BOOL testMode_MonitorCTRL_Print_FLAG;// Prints A2D read value when valid

//  ---Periods & Flags
extern int adc_sampling_period;
extern int monitor_print_period;
extern BOOL monitor_print_FLAG;
extern int i2c_wait_period;


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
	int i; DWORD res = 0; BYTE i2cStat=0; BYTE i2cAddr=0;
	//-------------------------------------General System--------------------------------------------//
	SimpleUart(0,SystemBaud);assign_stdio(0);				// Serial port 0 for Data
	SimpleUart(1,SystemBaud);								// Serial port 1 for Debug
	EnableSmartTraps();
	#ifdef _DEBUG
		InitGDBStubNoBreak( 1, 115200 );
	#endif
	OSChangePrio(MAIN_PRIO);								//Other
	EnableSerialUpdate();



	WAIT_FOR_KEYBOARD
	iprintf("\n\n\n\n\n\n\n\nINITIALIZATION\n");
	iprintf("Version Number --> %s\n", ftos(versionNo, 3));

	iprintf(" .Assigned %d Baud rate to UART 0 for STDIO\n", SystemBaud);
	createVERIFY_KEY();										// Using versionNo to update Flash DDBB


	//----------------------------------------DataLists----------------------------------------------//

	res = initializeValuesPSUsSnIs();		// Load psuList values from RAM or set them to 	default
	iprintf(" .Data Lists -> ");
	iprintf(" %s (0x%x)\n", (res==0?"OK":"ERROR - Set to Default"), res);
	for (i = 0; i<PSU_NUMBER; i++){			// Start all PSU objects in OFF status, regardless of the loaded status
		psuList[i].relayStatus = OFF;
		psuList[i].psuStatus = OFF;
		psuList[i].ReadyToConnect = false;
	}
	iprintf(" .All PSU objects in OFF status\n");


	//----------------------------------------ConfigCTRL----------------------------------------------//
	config_MonitorCTRL_SnI_FLAG = CONFIG_INIT_MONITORCTRL_SnI_FLAG;			// Supply & Internal voltages
	config_MonitorCTRL_PrintVals = CONFIG_INIT_MONITORCTRL_PRINT;			// Supply & Internal voltages																		// included in a2d loop or not
	config_AlarmCTRLUpdate_SnI_FLAG = CONFIG_INIT_ALARMCTRL_UPDATE_SnI_FLAG;// Includes/excludes SnI voltage																			// alarms when using updateAlarms(void)
	alarmUpdatePeriodx50MS = CONFIG_INIT_ALARMCTRL_UPDATE_PERIOD_x50MS;		// Period x50ms for alarm update task
	consoleOutputRDAC = CONFIG_INIT_CONSOLE_OUTPUT_RDAC;					// Toggles console messages for RDAC functions
	consoleOutputI2C = CONFIG_INIT_CONSOLE_OUTPUT_I2CnSPI;					// Toggles console messages for I2C and SPI functions
	// Pause Tasks
	idleMode_AlarmCTRL_Task_FLAG=CONFIG_INIT_PAUSE_TASKS;	// Sets Alarm Task into idle mode
	seqMode_MonitorCTRL_Task_FLAG=CONFIG_INIT_PAUSE_TASKS;	// Stops Monitor Task, pending it on seqMode_MonitorCTRLTask_Sem
	// Testing Booleans
	testMode_MonitorCTRL_Measure_FLAG = false;				// Uses testMode_Measure instead of reading from ADC
	testMode_SwitchOnCTRL_Task_FLAG = false; 				// Stores the PSUs waiting to be switched on in remainingPSUs[20]
	testMode_MonitorCTRL_Print_FLAG = false;				// Prints A2D read value when valid
	//Periods & Flags
	adc_sampling_period = ADC_SAMPLING_PERIOD;
	monitor_print_period = MONITOR_PRINT_PERIOD;
	monitor_print_FLAG = false;
	i2c_wait_period = I2C_WAIT_PERIOD;



	//---------------------------------------TimerInterruptions--------------------------------------//
	SetUpPITR(1650, 1); // Use PITR 1 (PIT0 messes with RTOS), Wait 1650 clocks (CLKref: 33MHz)
						// Divide by 2^1 from table 17-3 (CF5213RM.pdf), 10KHz
						// Interrupt in charge of A2D sampling, I2C waiting
	iprintf(" .Timed Interruption initialized\n");
	iprintf(" .Semaphores %s\n", (allSemInit()==OS_NO_ERR?"Initialized":"Initialization ERROR"));
	OSTimeDly(TICKS_PER_SECOND/2);


	//--------------------------------------I2C Bus Connection---------------------------------------//
	iprintf(" @I2C:\n");
	I2CInit(MCF5213_I2C_ADDRESS, I2C_FREQUENCY); 			// Initialize I2C with Predefined Address (0x16)
															// Set frequency division to 768 (66Mhz CF clock --> 85,94 Khz I2C bus)
															// See Table 22.3 Reference Manual (5213RM)
	iprintf("    -Initialized I2C address for MCF5213: 0x%x\n",MCF5213_I2C_ADDRESS);
	iprintf("    -Set I2C Frequency division: %x (MCF internal CLK / 1280)\n",I2C_FREQUENCY);
	iprintf("    -Checking I2C communications with SFs 1 to 6:\n");
	for (i = 0; i<6; i++){
		i2cAddr = i2CAddressArray[2*i];
		iprintf("        .Sending an I2C message to SF%d (addr: 0x%x) ->", i+1,i2cAddr);
		res = (configureSPI( INIT_SPI_ORDER, INIT_SPI_CPOL, INIT_SPI_CPOL, INIT_SPI_CLKRATE, i2cAddr)==I2C_OK);
		iprintf(" %s\n", (res?"OK":"ERROR"));
		OSTimeDly(TICKS_PER_SECOND/5);
	}
	iprintf("    -Checking I2C communications with AGC:\n");
	iprintf("        .Sending an I2C message to AGC (addr: 0x%x) ->", AGC_I2C_ADDRESS);
	res = ( configureSPI( INIT_SPI_ORDER, INIT_SPI_CPOL, INIT_SPI_CPHA, INIT_SPI_CLKRATE, AGC_I2C_ADDRESS)==I2C_OK);
	iprintf(" %s\n", (res?"OK":"ERROR"));
	OSTimeDly(TICKS_PER_SECOND/5);
	iprintf("    -Checking I2C communications with Bus Expander:\n");
	iprintf("        .Sending an I2C message to Bus Expander (addr: 0x%x) ->", EXPANDER_I2C_ADDRESS);
	res = (sendI2CMessage (expanderBusBuffer, 1, EXPANDER_I2C_ADDRESS)==I2C_OK);
	iprintf(" %s\n", (res?"OK":"ERROR"));
	OSTimeDly(TICKS_PER_SECOND/5);

	//-------------------------------------SPI Bus Connections---------------------------------------//
	iprintf(" @SPI:\n");
	res=0; i2cStat=0;
	iprintf("    -Setting all RDACs to minimum\n");
	for (i = 0; i<PSU_NUMBER; i++){
		iprintf("        .Sending SF%d_%s to allow adjust & min(I2C: 0x%x, SPI: 0x%d) ->", (i&0x1?(i-1)/2+1:i/2+1), (i&0x1?"A":"B"),i2CAddressArray[i], (i&0x1?1:2));
		setCtrlRDAC(0, 1, 0, (i&0x1?1:2), i2CAddressArray[i]);
		res = (getI2CResultRDAC()==I2C_OK);
		res &= (getCtrlRDAC((i&0x1?1:2), i2CAddressArray[i])==0x02);
		res &= (adjustRdac (i, RDAC_MAX_VALUE, 1/* not used, just for differentiation*/)!=0);
		iprintf(" %s\n", (res?"OK":"ERROR"));
		OSTimeDly(TICKS_PER_SECOND/5);
	}

	//-------------------------------------------GPIO------------------------------------------------//
	RESET_RHEOSTATS.function( PIN21_GPIO ); 	// RDAC Reset
	LED_OUT_ON.function( PIN7_GPIO ); 			// LED OUT
	iprintf(" .Pin 21 (TIN3) set as GPIO for RDAC Hardware Reset\n");
	iprintf(" .Pin 7 (IRQ4) set as GPIO for LED OUT\n");
	OSTimeDly(TICKS_PER_SECOND/5);

	//----------------------------------Analog to Digital Converter----------------------------------//
	Pins[13].function( PIN13_AN0 );	EnableAD();		// Configure the A2D pin nº13 as analog input for A2D conversion
	initMUX();										// Initializes all the Pins used for MUX control
	minAGC();
	readValuesAGC();								// Reads the initialized values in AGC's RDAC
	res= (getI2CResultAGC()) | !(getMinReachedAGC()==true);
	if(res==0)	{	iprintf(" .ADC initialized OK\n");		}
	else		{	iprintf(" .ADC initialization ERROR (AGC --> C:%s, Min:%s)\n", (i2cStat==I2C_OK?"+":"~"), (getMinReachedAGC()?"+":"~"));	}
	OSTimeDly(TICKS_PER_SECOND/5);

	//------------------------------------------  Relays  -------------------------------------------//
	res=initRelay()==I2C_OK;								// Relays disconnected just at the beginning
	if(res)	{	iprintf(" .Relays initialized (all Disconnected)\n");						}
	else	{	iprintf(" .Relays initialization ERROR (couldn't use I2C bus expander)\n");	}
	OSTimeDly(TICKS_PER_SECOND/5);

	//--------------------------------------Button Interruption--------------------------------------//
	SetUpIRQ1();
	iprintf(" .Button Interruption Set\n");
	OSTimeDly(TICKS_PER_SECOND/5);

    //----------------------------------------- RTOS Tasks-------------------------------------------//
	toggle_MonitorCTRL_PrintVals ( false );
	iprintf(" @Tasks:\n");
	OSSimpleTaskCreate( alarmTask, ALARM_PRIO );
	iprintf("    -alarmTask initialized\n");
	OSSimpleTaskCreate( monitorTask, MONITOR_PRIO );
	iprintf("    -MonitorTask initialized\n");
	OSTimeDly(TICKS_PER_SECOND/2);
	toggle_MonitorCTRL_PrintVals ( CONFIG_INIT_MONITORCTRL_PRINT );

	iprintf("APPLICATION INITIALIZED\n\nPress one key to begin \n\n");
	WAIT_FOR_KEYBOARD
}

