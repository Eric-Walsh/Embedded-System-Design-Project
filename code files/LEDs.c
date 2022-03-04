// *************************************************************************//
// Filename: LEDs.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs debug_signals.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches, a DAC, an ADC, 2 LEDS, and a 2x8 LCD   //
// Operation: initialzes the LEDs and handles the toggling of the LEDs//
// *************************************************************************//
#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"


//header for function taken from lcd_lib_4bit_20b.c file from class webpage
/*----------------------------------------------------------------------------
    Header for function in lcd_lib_4bit_20b.c            L.Aamodt
*----------------------------------------------------------------------------*/
void delayMs(uint32_t n);

/*----------------------------------------------------------------------------
    Initializes the LEDs			                     E.Walsh
*----------------------------------------------------------------------------*/
void init_LEDs(void) {
	// Enable clock to ports E
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Make 2 pins GPIO
	PORTE->PCR[LED1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[LED1_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[LED2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[LED2_POS] |= PORT_PCR_MUX(1);                   
	
	// Set ports to outputs
	PTE->PDDR |= MASK(LED1_POS) | MASK(LED2_POS);
	

	
	//flash lEDs for 1 second to make sure that they are initianlized
	control_LEDs(1, 1);
	delayMs(1000);
	control_LEDs(0, 0);
}

/*----------------------------------------------------------------------------
    Turns LEDs off or on                                 E.Walsh
*----------------------------------------------------------------------------*/
void control_LEDs(unsigned int led1_on, unsigned int led2_on) {
	if (led1_on) {
			PTE->PCOR = MASK(LED1_POS);
		
	} else {
			PTE->PSOR = MASK(LED1_POS);
	}
	if (led2_on) {
			PTE->PCOR = MASK(LED2_POS);
	}	else {
			PTE->PSOR = MASK(LED2_POS);
	} 
	
}	

/*----------------------------------------------------------------------------
    Turns on LEDs 					                     E.Walsh
*----------------------------------------------------------------------------*/
void toggle_LEDs(unsigned int led1_tog, unsigned int led2_tog) {
	if (led1_tog) {
			PTE->PTOR = MASK(LED1_POS);
	} 
	if (led2_tog) {
			PTE->PTOR = MASK(LED2_POS);
	} 
		
}	

