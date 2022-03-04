// *************************************************************************//
// Filename: Pit_Functions.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs ADC_Functions.c, DAC_Functions.c, Calculations.c, debug_signals.c, and Interface_functions.h //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches,a DAC, ADC, 2 LEDS, and a 2x8 LCD   //
// Operation: Initializes the PIT, handles PIT interrupts, and starts or stops the timer//
// *************************************************************************//
#include "MKL25Z4.h"
#include "DAC_Functions.h"
#include "gpio_defs.h"
#include "Calculations.h"
#include "ADC_Functions.h"
#include "Interface_Functions.h"


/*----------------------------------------------------------------------------
    Initializes the PIT on channel 0                      E.Walsh
*----------------------------------------------------------------------------*/
void Init_PIT0(unsigned period) {
		//ENable cloack to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
		//Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
		//Initialize PIT0 to count down from argument
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(250*period);
		//No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;

		//Generate Interrupts
	//PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	
		/* Enable INterrupts */
	/*NVIC_SetPriority(PIT_IRQn, 128); //0,64,128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);*/
}

/*----------------------------------------------------------------------------
    Initializes the PIT on channel 1                      E.Walsh
*----------------------------------------------------------------------------*/
void Init_PIT1(unsigned period) {
		//ENable cloack to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
		//Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
		//Initialize PIT1 to count down from argument
	PIT->CHANNEL[1].LDVAL = PIT_LDVAL_TSV(period);
	
		//No chaining
	PIT->CHANNEL[1].TCTRL &= PIT_TCTRL_CHN_MASK;

		//Generate Interrupts
	PIT->CHANNEL[1].TCTRL |= PIT_TCTRL_TIE_MASK;
	
		/* Enable INterrupts */
	NVIC_SetPriority(PIT_IRQn, 128); //0,64,128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn);
}

/*----------------------------------------------------------------------------
    Starts PIT for chosen channel                         E.Walsh
*----------------------------------------------------------------------------*/
void Start_PIT(int channel) {
		//Enable counter
	PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
}

/*----------------------------------------------------------------------------
    Stops PIT for chosen channel                          E.Walsh
*----------------------------------------------------------------------------*/
void Stop_PIT(int channel){
		//Enable Counter
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

/*----------------------------------------------------------------------------
    Handles PIT interrupt requests                        E.Walsh
*----------------------------------------------------------------------------*/
void PIT_IRQHandler(void) {
	DEBUG_PORT->PSOR = MASK(DBG_MAIN_POS);
		//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
		//check to see which channel triggered interrupt
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK){
			//Clear status flags for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK){
			//clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
		//Get the amount of beats counted from the ADC and sends the info to the user interface
		UpdateBPM(CalculateBPM(GetBeats()));
	}
	DEBUG_PORT->PCOR = MASK(DBG_MAIN_POS);
}
