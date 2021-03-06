/*
 * defineConstants.cpp
 *
 *	Contains most of the #define statements, which allows system configuration, addressing, choosing default values,
 *	and simplifies the syntax of certain dense modules in PSUControl.
 *
 *  Created on: 13-feb-2015
 *   Author: Alberto Ibarrondo Luis
 */

//_________________________________________RADAR INIT VOLTAGES_________________________________________________//

#define RADAR_INIT_VOLTS  		\
	{ 	/*SF1 A & B*/ 7,   8,	\
		/*SF2 A & B*/ 1.8, 3.3,		/*Not monitored for the moment, will be when backplane gets updated*/ \
		/*SF3 A & B*/ 5,   5,	\
		/*SF4 A & B*/ 5,   5,	\
		/*SF5 A & B*/ 24,  12,	\
		/*SF6 A & B*/ -5,  -12  }

#define RADAR_INIT_COUNTS  		\
	{ 	/*SF1 A & B*/ 834,  806,\
		/*SF2 A & B*/ 1005, 956,\
		/*SF3 A & B*/ 901,  900,\
		/*SF4 A & B*/ 902,  902,\
		/*SF5 A & B*/ 286,  675,\
		/*SF6 A & B*/ 977,  853  }

//____________________________________________CONFIGURATION____________________________________________________//
// Controller
#define CONFIG_INIT_MONITORCTRL_SnI_FLAG 		( false )// Supply and Internal voltages included in a2d loop
#define CONFIG_INIT_MONITORCTRL_PRINT			( true  )// Print measured values of PSUs every MONITOR_PRINT_PERIOD
#define CONFIG_INIT_ALARMCTRL_UPDATE_SnI_FLAG 	( true  )// Supply and Internal voltages included in alarm update
#define CONFIG_INIT_ALARMCTRL_UPDATE_PERIOD_x50MS  ( 2 ) // 100ms
#define CONFIG_INIT_PAUSE_TASKS					( true  )// Pauses both alarm and monitor tasks when initializing
#define CONFIG_INIT_CONSOLE_OUTPUT_RDAC			(  OFF  )// Toggle on/off console messages for I2C and SPI functions
#define CONFIG_INIT_CONSOLE_OUTPUT_I2CnSPI		(  OFF  )// Toggle on/off console messages for RDAC functions

// ADC
#define MAXIMUM_LEVEL_ADC 		( 0x0E80 )	// maximum A2D counts to label a test_measure as "valid". Otherwise, the AGC will go down
#define MIDSCALE_ADC 			( 0x0A80 )	// Objective for the second reading in A2D, configuring the AGC accordingly
#define MINIMUM_LEVEL_ADC 		( 0x0700 )	// minimum A2D counts to label a test_measure as "valid". Otherwise, the AGC will go up
#define ADC_SAMPLING_PERIOD		(  300   )	// x0.1 number of milliseconds between each sampling
#define MONITOR_PRINT_PERIOD	( 20000  )	// x0.1 number of milliseconds between each value printing

// I2C
#define I2C_FREQUENCY 			( 0x16   )	// Max is 100KHz
#define I2C_WAIT_PERIOD			( 16500  )	// divided by 33000 is the number of milliseconds to wait after each I2C sent message

// SPI
#define INIT_SPI_ORDER			( false  )	// MSB first (0)
#define INIT_SPI_CPOL			( false  )	// Clock Polarity LOW when idle (0)
#define INIT_SPI_CPHA			( false  )	// Data clocked in on leading edge (0)
#define INIT_SPI_CLKRATE		( 2      )	// 115KHz as default. Correspondence: 0 -> 1.8MHz; 1 -> 461KHz; 2 -> 115KHz; 3 -> 58KHz;

// Task Priorities
#define ALARM_PRIO			( MAIN_PRIO-1 )	// Alarm task will have higher priority than main task
#define MONITOR_PRIO 		( MAIN_PRIO-2 )	// Monitor task will have higher priority than main task and alarm task


//________________________________________________VALUES_______________________________________________________//
#define RDAC_MAX_VALUE 	 ( 1023 )			// Max & min values for RDAC Register
#define RDAC_MIN_VALUE 	 ( 0 )
#define INIT_VOLTAGE_POS ( 1.26 )			// Initial value for + PSUs' Rdac (set to min to minimize power consumption)
#define INIT_VOLTAGE_NEG ( -2.6 )			// Initial value for - PSUs' Rdac (set to min to minimize power consumption)

#define PSU_NUMBER 		 ( 12 )				// 0 to 9 are positive voltages, 10 and 11 are negative
#define SUP_NUMBER 		 ( 13 )				// Complete list below, in section PSUs&SnIs
#define ALARM_NUMBER 	 ( 4 )				// inferior and superior, for voltage and current
#define PROTOCOLS_NUMBER ( 3 )				// Shutdown, ModVolt and Message

#define TICKS_100MS 	 ( 2 )				// TICKS_PER_SECOND = 20



//_____________________________________________ADDRESSES_______________________________________________________//
#define MCF5213_I2C_ADDRESS 		( 0x20 )	// Controller. Can be changed from 0x07 to 0x78
#define EXPANDER_I2C_ADDRESS 		( 0x38 )	// I2C Expander - All address bits set to 0
#define AGC_I2C_ADDRESS 			( 0x28 )	// Automatic gain control I2C address

#define UPPER_SLAVE_SPI_ADDRESS 	( 0x1 )		// U202 - Upper slave SPI address (for PSUs with A on their names)
#define LOWER_SLAVE_SPI_ADDRESS 	( 0x2 )		// U302 - Lower slave SPI address (for PSUs with B on their names)
#define AGC_SPI_ADDRESS 			( 0x1 )		// Automatic gain control SPI address

#define PSUs_I2C_ADDRESS_ARRAY	{0x2F, 0x2F, 0x2E, 0x2E, 0x2D, 0x2D, 0x2C, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A}



//_______________________________________________ALARMS________________________________________________________//
// ALARM ARRAYS' ACCESS METHODS
#define _(a,b) 		( a+2*b )		// Arrays with 4 items. a=(INFERIOR/SUPERIOR); b=(VOLTAGE/CURRENT)
#define __(a,c) 	( a+4*c )		// Array with 8/12 items. a=ALARM NUMBER (0 to 3);
									// c=(PROTOCOL_SHUTDOWN/PROTOCOL_MOD_VOLTAGE/PROTOCOL_MESSAGE) for alarmProtocols.
#define ___(a,b,c) 	( a+2*b+4*c )	// Array with 8/12 items. a=(INFERIOR/SUPERIOR); b=(VOLTAGE/CURRENT);
									//		c=(PROTOCOL_SHUTDOWN/PROTOCOL_MOD_VOLTAGE/PROTOCOL_MESSAGE) for alarmProtocols.

// ALARM ARRAYS INDEXING
//inf_sup
#define INFERIOR 	( 0	)			// First alarm array index bit (input "a" of _ and ___ accessing methods)
#define SUPERIOR 	( 1 )
//vol_curr
#define VOLTAGE 	( 0 )			// Second alarm array index bit (input "b" of _ and ___ accessing methods)
#define CURRENT 	( 1 )

// Third alarm array index bit for alarmProtocols (input "c" of the accessing methods):
#define PROTOCOL_SHUTDOWN 	 ( 0 )	// (0) Shut down certain PSUs
#define PROTOCOL_MOD_VOLTAGE ( 1 )	// (1) Modify Voltage
#define PROTOCOL_MESSAGE 	 ( 2 )	// (2) Send Alarm Message


#define ALARM_MESSAGE "ALARM in %s %d. %s %s limit reached\n", (psu_sni==PSU_TYPE_LIST?"PSU":"SnI"), Num, (inf_sup==0?"Inferior": "Superior"), (volt_corr==0?"Voltage": "Current")


//______________________________________________PSUs&SUPs______________________________________________________//
// psu_sni
#define PSU_TYPE_LIST ( 1 )			// Used in functions to choose PSUs or SUPS
#define SnI_TYPE_LIST ( 0 )

// PSU OUTPUTS
#define SF1_A 			( 0 )
#define SF1_B 			( 1 )
#define SF2_A 			( 2 )
#define SF2_B 			( 3 )
#define SF3_A 			( 4 )
#define SF3_B 			( 5 )
#define SF4_A 			( 6 )
#define SF4_B 			( 7 )
#define SF5_A 			( 8 )
#define SF5_B 			( 9 )
#define SF6_A 			( 10 )
#define SF6_B 			( 11 )

// SUPPLY VOLTAGES
#define SUP_42V_UNREG 	( 0 )
#define SUP_35V_UNREG 	( 1 )
#define SUP_16V_UNREG  	( 2 )
#define SUP_32V_REG 	( 3 )
#define SUP_16V_REG 	( 4 )
#define SUP_12V_F_A 	( 5 )
#define SUP_12V_F_B 	( 6 )
#define SUP_12V_F_C 	( 7 )
#define SUP_12V_F_D 	( 8 )
#define SUP_n16_REG 	( 9 )
#define SUP_n20_UNREG 	( 10 )
#define SUP_VCC_3V3 	( 11 )
#define SUP_VCC_n12V 	( 12 )

//______________________________________________GPIO PINS______________________________________________________//
// Controller
#define SW_OUT_ON 			Pins[6]		// Button
#define LED_OUT_ON 			Pins[7]		// Led Out
#define RESET_RHEOSTATS 	Pins[21]	// HW Rheostats' reset
// MUX_ABCDE
#define A0_MUX_ABCDE		Pins[28]
#define A1_MUX_ABCDE		Pins[33]
#define A2_MUX_ABCDE		Pins[34]
#define EN_MUX_ABCDE		Pins[8]
// MUX_ADC
#define A0_MUX_ADC			Pins[15]
#define A1_MUX_ADC			Pins[16]
#define A2_MUX_ADC			Pins[17]
#define EN_MUX_ADC			Pins[18]

//________________________________________________MUXES________________________________________________________//
// MUXES
#define MUX_A 	( 0 )
#define MUX_B 	( 1 )
#define MUX_C 	( 2 )
#define MUX_D 	( 3 )
#define MUX_E 	( 4 )

//SAMPLING FUNCTIONS
#define FUNCTION_PSU_VOLTAGE 	( 0 )
#define FUNCTION_PSU_CURRENT 	( 1 )
#define FUNCTION_SnI_VOLTAGE 	( 2 )

// MUX I/O for each psu and sup					  0      1      2      3      4       5       6       7       8      9        10     11    12
#define POS_MUX_ABCDE_ARRAY	{/*PSU VOLTAGES*/ 	  6,     7,     0,     1,     4,      5,      0,      1,      2,     4,       2,     3,\
							 /*PSU CURRENTS*/     6,     7,   0xF,   0xF,     6,      7,      0,      1,      2,     5,       4,     5,\
							 /*SnI VOLTAGES*/     0,     1,     2,     3,     4,      0,      5,      6,      7,     0,       1,     2,     4}

#define POS_MUX_ADC_ARRAY	{/*PSU VOLTAGES*/ MUX_A, MUX_A, MUX_B, MUX_B, MUX_A,  MUX_A,  MUX_A,  MUX_A,  MUX_A, MUX_B,   MUX_E, MUX_E,\
							 /*PSU CURRENTS*/ MUX_C, MUX_C,   0xF,   0xF, MUX_B,  MUX_B,  MUX_C,  MUX_C,  MUX_C, MUX_B,   MUX_C, MUX_C,\
							 /*SnI VOLTAGES*/ MUX_D, MUX_D, MUX_D, MUX_D, MUX_D,      5,  MUX_D,  MUX_D,  MUX_D, MUX_E,   MUX_E, MUX_B, MUX_E}

//This are the new scale factors
// Scale Factor
 #define SCALE_FACTOR_ARRAY	{/*PSU VOLTAGES*/ 3.627, 3.631, 3.597, 3.627, 3.580,  3.581,  3.615,  3.591,  3.639, 3.662,  -2.676,-2.604,\
							 /*PSU CURRENTS*/ 0.926, 0.898,     0,     0, 0.840,  0.922,  0.889,  0.856,  0.962, 0.847,  -0.891,-0.491,\
							 /*SnI VOLTAGES*/     5,     3,  1.68,     3,     3, 3.68/3, 3.68/3, 3.68/3, 3.68/3, -1.68,      -2,     1,     -1}

#define DEFAULT_SCALE_FACTOR ( 4.3*4 )
// // Simple access to arrays
#define SCALE_FACTOR(samplFunc, num) ( scaleFactorArray[samplFunc*12+num] * DEFAULT_SCALE_FACTOR)
#define POSITION_MUX_ABCDE(samplFunc, num) ( posMuxABCDE[samplFunc*12+num] )
#define POSITION_MUX_ADC(samplFunc, num) 	( posMuxADC[samplFunc*12+num]   )



//__________________________________________DEFAULT PSU VALUES_________________________________________________//
#define DEFAULT_relayStatus_psu 	( false )				// Relay flag
#define DEFAULT_psuStatus_psu  		( false )				// PSU being used or not
#define DEFAULT_rdacValue_psu 		( INIT_VOLTAGE_POS )	// RDACs' programmed voltage value
#define DEFAULT_bridgeI2CAdr_psu 	( 0x2F )				// PSU I2C Bridge address - not in use. Use instead PSUs_I2C_ADDRESS_ARRAY

#define DEFAULT_alarmLimitValues_psu_pos {13, 17, 0.3, 3}	// Inferior (0) and superior (1) voltage alarm LIMITS (magnitude);
#define DEFAULT_alarmLimitValues_psu_neg {-17,-13,-3,-0.3}	//  and inferior (2) and superior (3) current alarm LIMITS. pos(0 to 9) and neg(10 and 11) PSUs
#define DEFAULT_alarmLimitTimes_psu		 {3, 30, 100, 300}	// Inferior (0) and superior (1) voltage alarm TIMES
															//  (Counter limits which trigger alarmStatus On/Off when reached);
															//  and inferior (2) and superior (3) current alarm TIMES.
#define DEFAULT_alarmProtocols_psu {0,0,0,0,0,0,0,0,0,0,0,0}// Activate(TRUE) or ignore(FALSE) each of the protocols when the alarm pops up:
															//  (0-3) Shut down certain PSUs, (4-7) Modify this PSU's Voltage, (8-11) Send Alarm Message.
															//  For more info see PSU_TYPE.cpp
#define DEFAULT_alarmProtocolShutdown_psu {}				// PSU list to shutdown in alarm protocol Shutdown. Bits 0 to B shutdown
															//  PSUs 1 to 12 if set to TRUE.
#define DEFAULT_alarmProtocolVoltage_psu 	{15,14,20,6}	// New values for this PSU's voltage when executed this alarm's Modify Voltage Protocol .
#define DEFAULT_alarmCounters_psu 			{0, 0, 0, 0}	// Variables increasing on each scanning period if alarmLimitReached is ON(TRUE),
															//  until they reach alarmLimitTimes.
															//  They also serve for shutting down alarms when alarmLimitReached is OFF(FALSE).
#define DEFAULT_alarmStatus_psu {false,false,false,false}	// FALSE: alarm hasn't been triggered, not performing any alarm protocols.
															//  TRUE: alarm is ON, performing alarm protocols.
#define DEFAULT_alarmLimitReached_psu {false,false,false,false}// FALSE: alarm hasn't reached the limit. If alarmStatus is ON, it will start
															//  decreasing the alarmCounter until alarmStatus is OFF(ntimes = alarmLimitTimes)
															//  TRUE: alarm has reached the limit, beginning to increase the alarmCounter
															//  until alarmStatus is ON (ntimes = alarmLimitTimes).

#define DEFAULT_alarmWatch_psu {false,false,false,false}	// FALSE: alarm is not configured, it will neither analyze the system nor trigger the alarmStatus ON.
															// TRUE: alarm is configured and working, will analyze the system, set the alarmStatus ON
															//  when a limit is reached, and execute the defined alarmProtocols
#define DEFAULT_rShunt_psu 			( 820 )					// Internal resistor used for RDAC configuration
#define DEFAULT_iOffset_psu {0.006,0.005,0,0,0.038,0.007,0.019,0.035,0.002,0,-0.005,-0.04}// Current measured without charge

#define DEFAULT_vOut_psu 			( 0 )					// ADC value of the output voltage
#define DEFAULT_vMean_psu			( 0 )					// Mean of voltages
#define DEFAULT_cOut_psu 			( 0 )					// ADC value of the output current
#define DEFAULT_cMean_psu			( 0 )					// Mean of currents

#define DEFAULT_initializationTimer_psu ( 10 )				// x100ms initial count for PSU switching-on


//_______________________________________________DEFAULT SnI VALUES____________________________________________//
#define DEFAULT_sniStatus_sni 				( true )		// SnI being used or not
#define DEFAULT_nominalVoltage_sni {42, 35, 16, 32, 16, 12, 12, 12, -16, -20, 12, 3.3, 12, -12}	// desired voltage value
#define SnI_INF_PERC_ALARM_VALUE 			( 0.8 )			// Inferior voltage limit for auxiliary supplies' alarm (percentage)
#define SnI_SUP_PERC_ALARM_VALUE 			( 1.2 )			// Superior voltage limit for auxiliary supplies' alarm (percentage)
															// 	Current limit values are not used
#define DEFAULT_alarmLimitTimes_sni	{100, 100, 100, 100}	// Inferior (0) and superior (1) voltage alarm TIMES
															//  (Counter limits which trigger alarmStatus On/Off when reached);
															//  and inferior (2) and superior (3) current alarm TIMES.
#define DEFAULT_alarmProtocols_sni  {1,0,1,0,0,1,0,1,0,1,0,0}// Activate(TRUE) or ignore(FALSE) each of the protocols when the alarm pops up:
															//  (0-3) Shut down certain PSUs, (4-7) Modify this PSU's Voltage, (8-11) Send Alarm Message.
															//  For more info see PSU_TYPE.cpp
#define DEFAULT_alarmProtocolShutdown_sni 	{0, 0, 0, 0}	// PSU list to shutdown in alarm protocol Shutdown. Bits 0 to B shutdown
															//  PSUs 1 to 12 if set to TRUE.
#define DEFAULT_alarmCounters_sni 			{0, 0, 0, 0}	// Variables increasing on each scanning period if alarmLimitReached is ON(TRUE),
															//  until they reach alarmLimitTimes.
															//  They also serve for shutting down alarms when alarmLimitReached is OFF(FALSE).
#define DEFAULT_alarmStatus_sni {false,false,false,false}	// FALSE: alarm hasn't been triggered, not performing any alarm protocols.
															//  TRUE: alarm is ON, performing alarm protocols.
#define DEFAULT_alarmLimitReached_sni {false,false,false,false}	// FALSE: alarm hasn't reached the limit. If alarmStatus is ON, it will start
															//  decreasing the alarmCounter until alarmStatus is OFF(ntimes = alarmLimitTimes)
															//  TRUE: alarm has reached the limit, beginning to increase the alarmCounter
															//  until alarmStatus is ON (ntimes = alarmLimitTimes).

#define DEFAULT_alarmWatch_sni {false,false,false,false}	// FALSE: alarm is not configured, it will neither analyze the system nor trigger the alarmStatus ON.
															// TRUE: alarm is configured and working, will analyze the system, set the alarmStatus ON
															//  when a limit is reached, and execute the defined alarmProtocols
#define DEFAULT_vOut_sni 			( 0 )					// ADC value of the output voltage



//_______________________________________________AUXILIARY____________________________________________//
#define ERASE_CONSOLE iprintf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
#define EXIT 'e'
#define OFF				 ( false )
#define ON				 ( true )
