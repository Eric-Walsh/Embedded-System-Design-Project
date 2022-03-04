// *************************************************************************//
// Filename: DAC_functions.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs PIT_Functions.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches, a DAC, an ADC, 2 LEDS, and a 2x8 LCD   //
// Operation:  Initializes the DAC. //
// *************************************************************************//
#include "MKL25Z4.h"
#include "PIT_Functions.h"

#define DAC_POS (30)
#define DAC_RESOLUTION (4096)


/*----------------------------------------------------------------------------
    Initializes the DAC                            E.Walsh
*----------------------------------------------------------------------------*/
void Init_DAC(void){
	//Enable clock to DAC and Port E
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	//Set pin signal type to analog
	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[DAC_POS] |= PORT_PCR_MUX(0);
	
	//Disable buffer mode
	DAC0->C1 =0;
	DAC0->C2 =0;
	
	//Enable DAC, select VDDA as referance voltage
	DAC0->C0 = DAC_C0_DACEN_MASK |DAC_C0_DACRFS_MASK;
}

/*----------------------------------------------------------------------------
    Produces a triangle wave                       E.Walsh
*----------------------------------------------------------------------------*/
void Triangle_Output(void){ 
	int i = 0;
	int shift = 1;
	
	while(1){
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(i);
		DAC0->DAT[0].DATH = DAC_DATH_DATA1(i >>8);
		
		i+= shift;
		if(i == 0){
			shift = 1;
		} else if (i == DAC_RESOLUTION-1){
			shift = -1;
		}
	}
}

