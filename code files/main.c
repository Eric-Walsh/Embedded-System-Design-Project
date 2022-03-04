// ***************************************************************************************************************//
// Filename: main.c 																							  //
// Author: E.Walsh 																								  //
// Version: 3/14/21 written 																					  //
// Processor: NXP MKL25Z4 																						  //
// Compiler: Keil uVision5 																						  //
// Library: CMSIS core and device startup 																		  //
// also needs ADC_Functions.c, DAC_Functions.c, Pit_Functions.c, debug_signals.c, LEDs.c, and StartUp_functions.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches,a DAC, ADC, 2 LEDS, and a 2x8 LCD  			  //
// ***************************************************************************************************************//
#include "MKL25Z4.h"
#include "PIT_Functions.h"
#include "ADC_Functions.h"
#include "DAC_Functions.h"
#include "gpio_defs.h"
#include "StartUp_Functions.h"
#include "LEDs.h"


#define MASK(x) (1UL << (x))
extern void init_debug_signals(void);
void delayMs(uint32_t n);


int main(void){
	init_All(1, 1, 1, 1, 1000, 239999999, 0, 1);
	Start_PIT(0);
	Start_PIT(1);
	while(1){
    	
	}
}
