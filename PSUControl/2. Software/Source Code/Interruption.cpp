/*
 * Interruption.cpp
 *
 *	Definition of ISRs for timer and button interruptions
 *
 *  Created on: 15-abr-2015
 *      Author: Alberto Ibarrondo Luis
 */


#include "Headers.h"
#include "Controller/Controller.h"		// All the Controller Methods

extern "C" {
//
// This function sets up the 5213 interrupt controller
//
void SetIntc(long func, int vector, int level, int prio);
}



//==============================================VARIABLES================================================//
// Imported
extern OS_SEM monitorSem;					// Controls monitorTask
extern OS_SEM waitI2CSem;					// Controls wait after sending I2C message
extern BOOL monitor_print_FLAG;				// Prints values periodically

// Counters
DWORD pitr_count = 0 ;						// Simple counter for PIT1. It's also a way to measure time (1 count = 0,1 msec)

extern int adc_sampling_period;
extern int monitor_print_period;

int pitr_adc_sampling_count = adc_sampling_period;
int pitr_monitor_print = monitor_print_period;

// Button
BOOL buttonActionFLAG = false;				// used to execute buttonActionFLAG()


//========================================INTERRUPTION MACROS============================================//
INTERRUPT( irq_button, 0x2100 )
{
	//
	// When an interrupt occurs, this ISR clears the IRQ1 flag and toggles the
	// buttonActionFLAG,which triggers buttonAction() in the next alarmTask loop
	//

	buttonActionFLAG = true;
}


INTERRUPT( pitr_10KHz_isr, 0x2300 )
{
	WORD tmp = sim.pit[1].pcsr; // Use PIT 1

	// Clear PIT 1
	tmp &= 0xFF0F; 	// Bits 47 cleared
	tmp |= 0x0F; 	// Bits 03 set
	sim.pit[1].pcsr = tmp;


	pitr_adc_sampling_count--;		// Post into monitorSem every ADC_SAMPLING_PERIOD*0.1 milliseconds
	if (pitr_adc_sampling_count == 0){	OSSemPost(& monitorSem);
		pitr_adc_sampling_count = adc_sampling_period;				}

	pitr_monitor_print--;		// Sets true (to print values in MonitorTask) every MONITOR_PRINT_PERIOD*0.1 milliseconds
	if (pitr_monitor_print == 0){	monitor_print_FLAG = true;
		pitr_monitor_print = monitor_print_period;					}

	pitr_count++;
}

//======================================INTERRUPTION METHODS=============================================//
///////////////////////////////////////////////////////////////////////
// SetUpPITR PIT setup function. See chapter 17 of the 5213 reference
// manual for details.
///////////////////////////////////////////////////////////////////////

void SetUpPITR(WORD clock_interval, BYTE pcsr_pre /* See table 17-3 in the users manual for bits 8-11*/) {
	WORD tmp;

	// Populate the interrupt vector in the interrupt controller

	SetIntc((long) &pitr_10KHz_isr, 55 + 1, 3 /* Level 3 */, 3 /* PRIO 3 */);
	sim.pit[1].pmr = clock_interval; // Set the PIT modulus
	// value
	tmp = pcsr_pre;
	tmp = (tmp << 8) | 0x0F;
	sim.pit[1].pcsr = tmp; // Set the system clock
	// divisor to 2
}


///////////////////////////////////////////////////////////////////////
// SetUpIRQ1 IRQ1 setup function. See 5213 reference manual for details.
///////////////////////////////////////////////////////////////////////

void SetUpIRQ1( void ){
	SW_OUT_ON.function( PIN6_IRQ1 ); 	// Configure pin 6 as IRQ1
	sim.eport.eppar |= 0x0004; 			// Configure IRQ1 to trigger an interrupt on rising edge sensitivity
	sim.eport.epddr &= ~0x02; 			// Configure IRQ1 as input
	sim.eport.epier = 0x02; 			// Enable IRQ1
	SetIntc( ( long ) &irq_button, 1, 1, 1 );
}

