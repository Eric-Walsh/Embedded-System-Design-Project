// *************************************************************************//
// Filename: switches.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs Interface_functions.c and debug_signals.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches,a DAC, ADC, 2 LEDS, and a 2x8 LCD //
// Operation: Initializes the switch inputs on port A and handles the port A interrupts. //
// *************************************************************************//
#include <MKL25Z4.H>
#include "switches.h"
#include "gpio_defs.h"
#include "Interface_Functions.h"


/*----------------------------------------------------------------------------
    Initializes the switches on Port A                      E.Walsh
*----------------------------------------------------------------------------*/
void init_Switch(void) {
	
	SIM->SCGC5 |=  SIM_SCGC5_PORTA_MASK; /* enable clock for port A */

	/* Select GPIO and enable pull-up resistors and interrupts 
		on falling edges for pins connected to switches */
	PORTA->PCR[SW1_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	PORTA->PCR[SW2_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	PORTA->PCR[SW3_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);
	PORTA->PCR[SW4_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a);

	/* Set port A switch bit to inputs */
	PTA->PDDR &= ~MASK(SW1_POS);
	PTA->PDDR &= ~MASK(SW2_POS);
	PTA->PDDR &= ~MASK(SW3_POS);
	PTA->PDDR &= ~MASK(SW4_POS);

	/* Enable Interrupts */
	NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PORTA_IRQn); 
	NVIC_EnableIRQ(PORTA_IRQn);
}

/*----------------------------------------------------------------------------
    Handles port A Interrupt requests                       E.Walsh
*----------------------------------------------------------------------------*/
void PORTA_IRQHandler(void) {  
	DEBUG_PORT->PSOR = MASK(DBG_ISR_POS);
	// clear pending interrupts
	NVIC_ClearPendingIRQ(PORTA_IRQn);

	if ((PORTA->ISFR & MASK(SW1_POS))) {
		Input(1);

	} else if ((PORTA->ISFR & MASK(SW2_POS))) {
		Input(2);

	} else if ((PORTA->ISFR & MASK(SW3_POS))) {
		Input(3);

	} else if ((PORTA->ISFR & MASK(SW4_POS))) {
		Input(4);
	}
	// clear status flags 
	PORTA->ISFR = 0xffffffff;
	DEBUG_PORT->PCOR = MASK(DBG_ISR_POS);
}
// **********************************************************************************************   
