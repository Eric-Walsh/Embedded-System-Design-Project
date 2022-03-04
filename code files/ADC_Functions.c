// *******************************************************************************************************//
// Filename: ADC_Functions.c 																			  //
// Author: E.Walsh 																						  //
// Version: 3/14/21 written 																			  //
// Processor: NXP MKL25Z4 																			      //
// Compiler: Keil uVision5 																				  //
// Library: CMSIS core and device startup 																  //
// also needs DAC_Functions.c, Pit_Functions.c, debug_signals.c, Calculations.c, Recordings.c, and LEDs.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches,a DAC, ADC, 2 LEDS, and a 2x8 LCD 	  //
// Operation: Initilizes the ADC, handles ADC interrupts, and records the input 						  //
// Blinks LED for every heartbeat detected 																  //
// *******************************************************************************************************//
#include "MKL25Z4.h"
#include "DAC_Functions.h"
#include "Pit_Functions.h"
#include "gpio_defs.h"
#include "Calculations.h"
#include "Recordings.h"
#include "LEDs.h"
#include "ADC_Functions.h"
#define ADC_POS (20)

//global values
int refCount, risingFlag, beatCount, recordFlag, recordCount, waveFile; 
uint32_t magnitude, Low, High;

/*----------------------------------------------------------------------------
    Initializes the ADC                                  E.Walsh
*----------------------------------------------------------------------------*/
void Init_ADC(void){
		//enable clock for ADC and Port E
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    //Select analog for pin
    PORTE->PCR[ADC_POS] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[ADC_POS] |= PORT_PCR_MUX(0);
	
		//Configure the SIM SOPT7 register
		SIM->SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN(1); //Alternate trigger selected
    	SIM->SOPT7 |= SIM_SOPT7_ADC0PRETRGSEL(0); //Pre-trigger A
    	SIM->SOPT7 |= SIM_SOPT7_ADC0TRGSEL(4); //PIT0 trigger 
		
		//configure the ADC SC1A register 
		ADC0_SC1A |= ADC_SC1_AIEN(1); //conversion complete inturrupt enabled
		ADC0_SC1A |= ADC_SC1_DIFF(0); // single-ended conversions and input channels
		ADC0_SC1A &= ~ADC_SC1_ADCH_MASK; //DADP0 input
		
		//configure the ADC CFG1 register
		ADC0_CFG1 |= ADC_CFG1_ADLPC(0); //Normal power configuration
		ADC0_CFG1 |= ADC_CFG1_ADIV(1); //divide ratio 2
		ADC0_CFG1 |= ADC_CFG1_ADLSMP(0); // Short sample time
		ADC0_CFG1 |= ADC_CFG1_MODE(1); // single ended 12-bit conversion
		ADC0_CFG1 |= ADC_CFG1_ADICLK(1); // (bus clock)/2
		
        //configure the ADC SC2 register
		ADC0_SC2 |= ADC_SC2_ADTRG(1); //Hardware trigger 
		ADC0_SC2 |= ADC_SC2_ACFE(0); // No compare function
		ADC0_SC2 |= ADC_SC2_DMAEN(0); // DMA disabled
		ADC0_SC2 |= ADC_SC2_REFSEL(0); // Default voltage reference pin pair
		
        //configre the ADC SC3 register
		ADC0_SC3 |= ADC_SC3_ADCO(0); // AVGE = 1 after initiating conversion
		ADC0_SC3 |= ADC_SC3_AVGE(1); // hardware average function enabled
		ADC0_SC3 |= ADC_SC3_AVGS(0); // 4 sampled averaged
    
    /* Enable Interrupts */
	  NVIC_SetPriority(ADC0_IRQn, 3); //0,64,128 or 192
	  NVIC_ClearPendingIRQ(ADC0_IRQn);
	  NVIC_EnableIRQ(ADC0_IRQn);
	
	//initialize global values
	//refCount is used to count the number of values going into the reference array
	refCount = 0;
	//rising flag is used to signal if the value coming into the ADC is rising or falling
	risingFlag = 0;
	//Counts the number of beats detected by the ADC
	beatCount = 0;
	//flag used to signal if the incoming wave is being recorded or not
	recordFlag = 0;
	//counts the number of values that have been recorded
	recordCount = 0;
}

/*----------------------------------------------------------------------------
    Handles ADC interrupt requests                        E.Walsh
*----------------------------------------------------------------------------*/
void ADC0_IRQHandler(void) {
	DEBUG_PORT->PSOR = MASK(DBG_ISR_POS);
	int32_t n = 0;
	
		//clear pending IRQ
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	
	
		//read result
	n = ADC0_RA;
	//will calculate reference values to find beats as long as refCount is less than 1000
	if(refCount < 1000){
		//send n to be saved for reference
		ReferenceWave(n, refCount);
		refCount++;
		beatCount = 0;
	} else if(refCount == 1000){
		//find the high and low threshholds

		//magnitude of the wave found by subracting the biggest and smallest detected value
		magnitude = FindMax() - FindMin();

		//the Low threshold used to determine if the wave is falling or rising. 0.7*(magnitude+min)
		Low = FindLowThresh(magnitude, FindMin());

		//The high threshold used to determine if the wave is falling or rising. 0.8*(magnitude+min)
		High = FindHighThresh(magnitude, FindMin());
		refCount++;
	} else if (refCount > 1000){
		//increases the count of beats once the rising flag is set and reset and flashes the LED for every beat
		if(n > High && risingFlag != 1){
			risingFlag = 1;
			beatCount++;
			control_LEDs(1,0);
		} else if(n < Low && risingFlag == 1){
			 risingFlag = 0; 
			 control_LEDs(0,0);
		}
	}
    
	//sends the captured data to to be saved to the selected file as long as there's room in the array
	if(recordFlag && recordCount < 1000){
		//send the vales of n to be saved 
		control_LEDs(0,1);
		if(waveFile == 1){
			recordFile1(n, recordCount);
		} else if (waveFile == 2){
			recordFile2(n, recordCount);
		}
		recordCount++;
	} else if (recordCount == 1000){
		control_LEDs(0,0);
		StopRecording();
	}
	
	DEBUG_PORT->PCOR = MASK(DBG_ISR_POS);
}

/*----------------------------------------------------------------------------
    Returns the number of beats detected                  E.Walsh
*----------------------------------------------------------------------------*/
int GetBeats(void){
	int proxy = beatCount;
	beatCount = 0;
	return proxy;
}

/*----------------------------------------------------------------------------
    Starts recording the input			                  E.Walsh
*----------------------------------------------------------------------------*/
void StartRecording(int fileNum){
	//waveFile is the array that the values will be saved to
	waveFile = fileNum;
	recordFlag = 1;
	recordCount = 0;
}

/*----------------------------------------------------------------------------
    Stops recording the input			                  E.Walsh
*----------------------------------------------------------------------------*/
void StopRecording(void){
	recordFlag = 0;
}

/*----------------------------------------------------------------------------
    Resets the thresholds				                  E.Walsh
*----------------------------------------------------------------------------*/
void ResetThresh(void){
	refCount = 0;
}
