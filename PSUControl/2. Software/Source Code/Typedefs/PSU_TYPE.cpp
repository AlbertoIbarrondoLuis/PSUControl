/*
 * PSU_TYPE.cpp
 *
 *	Power Supply Unit Type, contains all the data regarding the 12 supply units being
 *	configured and monitored.
 *
 *	The list of this objects is created and handled in Controller/DataLists.cpp
 *
 *
 *  Created on: 18-feb-2015
 *   Author: Alberto Ibarrondo
 */

#ifndef PSU_TYPE_FILE
#define PSU_TYPE_FILE
#include "basictypes.h"

typedef struct PSU_TYPE {


	//----------General Status----------//
	BOOLEAN psuStatus;					// PSU being used or not
	BOOLEAN relayStatus;				// Relay connected or not
	float progVolt;						// RDACs' programmed value in volts
	float progCounts;					// RDACs' register value

	//------------Addressing------------//
	BYTE bridgeI2CAdr;					// PSU I2C Bridge address
	BYTE rdacAdr;						// RDAC Slave SPI address

	//-----------Alarm Arrays-----------//
	//First array index bit: INFERIOR(0)/SUPERIOR(1).
	//Second array index bit: VOLTAGE(0)/CURRENT(1).
	//Following array index bits: variable.
	//
	//Methods for array access:
	// _(a, b) --> Arrays with 4 items. a=(INFERIOR/SUPERIOR); b=(VOLTAGE/CURRENT)
	// __(a, b, c) --> Arrays with 12 items. a=(INFERIOR/SUPERIOR); b=(VOLTAGE/CURRENT)
	//		c=(PROTOCOL_SHUTDOWN/PROTOCOL_MOD_VOLTAGE/PROTOCOL_MESSAGE) for alarmProtocols.

	float alarmLimitValues[4];			// inferior (0) and superior (1) voltage alarm LIMITS (magnitude);
										// and inferior (2) and superior (3) current alarm LIMITS.



	int alarmLimitTimes[4];				// inferior (0) and superior (1) voltage alarm TIMES
										// (Counter limits which trigger alarmStatus On/Off when reached);
										// and inferior (2) and superior (3) current alarm TIMES.

	BOOL alarmProtocols[12];			// Activate(TRUE) or ignore(FALSE) each of the protocols when the alarm pops up:
										// c=(0) Shut down certain PSUs, (1) Modify this PSU's Voltage, (2) Send Alarm Message.

	WORD alarmProtocolShutdown[4];		// PSU list to shutdown in alarm protocol Shutdown.
										// Bits 0 to B shutdown PSUs 1 to 12 if set to TRUE.

	float alarmProtocolVoltage[4];		// New values for this PSU's voltage when executed this alarm's Modify Voltage Protocol .

	int alarmCounters[4];				// Variables increasing on each scanning period if alarmLimitReached is ON(TRUE), until they reach alarmLimitTimes.
										// They also serve for shutting down alarms when alarmLimitReached is OFF(FALSE).

	BOOL alarmStatus[4];				// FALSE: alarm hasn't been triggered, not performing any alarm protocols.
										// TRUE: alarm is ON, performing alarm protocols.

	BOOL alarmLimitReached[4];			// FALSE: alarm hasn't reached the limit. If alarmStatus is ON, it will start
										// decreasing the alarmCounter until alarmStatus is OFF(ntimes = alarmLimitTimes)
										// TRUE: alarm has reached the limit, beginning to increase the alarmCounter
										// until alarmStatus is ON (ntimes = alarmLimitTimes).

	BOOL alarmWatch[4];					// FALSE: alarm is not configured, it will neither analyze the system nor trigger the alarmStatus ON.
										// TRUE: alarm is configured and working, will analyze the system, set the alarmStatus ON
										// when a limit is reached, and execute the defined alarmProtocols


	//----------Internal Values---------//
	int rShunt;							// Internal resistor value used for RDAC configuration
	float iOffset;						// Current measured without charge

	//------------ADC values------------//
	float vOut;							// ADC value of the output voltage
	float vMean;						// Mean of voltages
	float cOut;							// ADC value of the output current
	float cMean;						// Mean of currents

	//-----------Initialization---------//
	int initializationTimer;  			// x100ms initial count for PSU switching-on
	BOOL ReadyToConnect;				// Self-explanatory


	//-------------FLASH----------------//
	DWORD VerifyKey;					// Verification key for stored data in FLASH memory
};
#endif
