// *************************************************************************//
// Filename: StartUp_Functions.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs ADC_Functions.c, LCD_Functions.c, DAC_Functions.c, Pit_functions.c, LEDs.c, and Interface_Functions.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches, a DAC, an ADC, 2 LEDS, and a 2x8 LCD   //
// Operation: Initializes all hardware adn interrupts needed for the project//
// *************************************************************************//
#include "LEDs.h"
#include "LCD_Functions.h"
#include "switches.h"
#include "PIT_Functions.h"
#include "ADC_Functions.h"
#include "DAC_Functions.h"
#include "Interface_Functions.h"

extern void init_debug_signals(void);

/*----------------------------------------------------------------------------
    Initializes the TPM                                          L.Aamodt
*----------------------------------------------------------------------------*/
void Init_TPM1(int32_t rate) {    // Initialize timer zero
			// Turn on TPM1 bus clock
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
			// Turn on clock that TPM counts
	SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);  	// select MCGPLLCLK/2
	
			// Load the counter reload value MOD, assuming a prescale of 2
	//TPM1->MOD = (F_TPM_CLOCK/(rate*2))-1;

	TPM1->SC |= TPM_SC_PS(1);     		// TPM1 prescale set to 2
	TPM1->SC |= TPM_SC_CMOD(1);  		// enable TPM1 counting
	TPM1->SC |= TPM_SC_TOF_MASK;		// reset timer overflow flag
}

/*----------------------------------------------------------------------------
    Initializes all the required hardware if flag is set         E.Walsh
*----------------------------------------------------------------------------*/
void init_All(unsigned int ADCinit, unsigned int DACinit, unsigned int SWITCHinit, unsigned int LCDinit, unsigned period1, unsigned period2, int32_t rate, unsigned int LEDinit){
    init_debug_signals();
	__enable_irq();
    if(ADCinit){
        Init_ADC();
    }
    if(DACinit){
        Init_DAC();
    }
    if(SWITCHinit){
        init_Switch();
    }
    if(LCDinit){
				SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;
				SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
        LCD_init();
				LCD_command(0x01);
        init_UI();
    }
    if(period1 != 0){
        Init_PIT0(period1);
    }
    if(period2 != 0){
        Init_PIT1(period2);
    }
    if(rate != 0){
        Init_TPM1(rate);
    }
    if(LEDinit){
        init_LEDs();
    }
    toggle_LEDs(0,1);
}
