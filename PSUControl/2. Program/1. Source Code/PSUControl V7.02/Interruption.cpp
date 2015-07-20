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

// Counters
DWORD pitr_count_zero;
DWORD pitr_count_adc_sampling;				// Simple counter for PIT1. It's also a way to measure time (1 count = 1 msec)

// Button
BOOL buttonActionFLAG = false;					// used to execute buttonActionFLAG()


//========================================INTERRUPTION MACROS============================================//
INTERRUPT( irq_button, 0x2100 )
{
	//
	// When an interrupt occurs, this ISR clears the IRQ1 flag and toggles the
	// buttonActionFLAG,which triggers buttonAction() in the next alarmTask loop
	//
	sim.eport.epfr |= 0x02; // Clear IRQ1 flag
	buttonActionFLAG = true;
}


INTERRUPT( pitr_adc_sampling, 0x2300 )
{
	WORD tmp = sim.pit[1].pcsr; // Use PIT 1

	// Clear PIT 1
	tmp &= 0xFF0F; // Bits 47 cleared
	tmp |= 0x0F; // Bits 03 set
	sim.pit[1].pcsr = tmp;
	OSSemPost(& monitorSem);
	pitr_count_adc_sampling++;
}

INTERRUPT( my_pitr_func_zero, 0x2200 )
{
	WORD tmp = sim.pit[0].pcsr; // Use PIT 0

	tmp &= 0xFF0F; // Bits 47 cleared
	tmp |= 0x0F; // Bits 03 set
	sim.pit[0].pcsr = tmp;
	//
	// You can add your ISR code here
	// Do not call any RTOS function with pend or init in the function
	// name
	// Do not call any functions that perform a system I/O read,
	// write, printf, iprint, etc.
	//
	pitr_count_zero++;
}

//======================================INTERRUPTION METHODS=============================================//
///////////////////////////////////////////////////////////////////////
// SetUpPITR PIT setup function. See chapter 17 of the 5213 reference
// manual for details.
///////////////////////////////////////////////////////////////////////

void SetUpPITR(int pitr_ch, WORD clock_interval, BYTE pcsr_pre /* See table 17-3 in the users manual for bits 8-11*/) {
	WORD tmp;
	//
	// Populate the interrupt vector in the interrupt controller
	//
	SetIntc((pitr_ch?((long) &pitr_adc_sampling):((long) &my_pitr_func_zero)), 55 + pitr_ch, 3 /* IRQ 3 */, (pitr_ch?3:2));
	sim.pit[pitr_ch].pmr = clock_interval; // Set the PIT modulus
	// value
	tmp = pcsr_pre;
	tmp = (tmp << 8) | 0x0F;
	sim.pit[pitr_ch].pcsr = tmp; // Set the system clock
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

