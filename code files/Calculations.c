// *************************************************************************//
// Filename: ADC_Functions.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs ADC_Functions.c, Pit_Functions.c, and Interface_functions.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches,a DAC, ADC, 2 LEDS, and a 2x8 LCD //
// Operation: Calculates the reference thresholds, calculates the BPM, and changes the sample rates //
// Stores the reference wave and sample rate arrays //
// *************************************************************************//
#include <MKL25Z4.h>
#include "ADC_Functions.h"
#include "PIT_Functions.h"
#include "Interface_Functions.h"

//the array that stores the reference wave
uint32_t refWave[1000];

//array that stores the sample rates
const uint32_t sample_rate[] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000};

//global variable to indicate the sample rate that's currently being used
int currentRate;

/*----------------------------------------------------------------------------
    Initializes the sample rate to be 1000                 E.Walsh
*----------------------------------------------------------------------------*/
void init_SampleRate(void){
    currentRate = 19;
}

/*---------------------------------------------------------------------------
    changes the sample rate based on user input. 
    Won't allow a user to go over 10000 or under 50         E.Walsh
*----------------------------------------------------------------------------*/
void ChangeSampleRate(int change){
    //change is either 1 or -1 depending on the switch pressed. 
    if(currentRate != 0 && change == -1){
        currentRate += change;
    } else if (currentRate != 37 && change == 1){
        currentRate += change;
    }
    //Updates PIT0 with the new rate and restarts PIT0 and PIT1
    UpdateSampleRate(currentRate);
    Stop_PIT(0);
		Stop_PIT(1);
    Init_PIT0(sample_rate[currentRate]);
    Start_PIT(0);
		Start_PIT(1);
    ResetThresh();
}

/*----------------------------------------------------------------------------
    Puts values into the reference wave                      E.Walsh
*----------------------------------------------------------------------------*/
void ReferenceWave(uint32_t value, int count){
    if(count < 1000){
    refWave[count] = value;
    }
}

/*----------------------------------------------------------------------------
    Find the largest vlaue in the wave                       E.Walsh
*----------------------------------------------------------------------------*/
uint32_t FindMax(void){
    uint32_t max = 0;
    for(int i = 0; i < 1000; i++){
        if(refWave[i] > max){
            max = refWave[i];
        }
    }
    return max;
}

/*----------------------------------------------------------------------------
    Find the smallest vlaue in the wave                      E.Walsh
*----------------------------------------------------------------------------*/
uint32_t FindMin(void){
    uint32_t min = refWave[0];
    for(int i = 0; i < 1000; i++){
        if(refWave[i] < min){
            min = refWave[i];
        }
    }
    return min;
}

/*----------------------------------------------------------------------------
    Calculates the low threshold
    70% of the sum of the magnitude and the min               E.Walsh
*----------------------------------------------------------------------------*/
uint32_t FindLowThresh(uint32_t magnitude, uint32_t min){
    return 0.60*(magnitude + min);
}

/*----------------------------------------------------------------------------
    Calculates the high threshold
    80% of the sum of the magnitude and the min               E.Walsh
*----------------------------------------------------------------------------*/
uint32_t FindHighThresh(uint32_t magnitude, uint32_t min){
    return 0.75*(magnitude + min);
}

//calculates the BPM by taking the count of beats in 15 seconds and multiplying by 4
/*----------------------------------------------------------------------------
    Calculates the BPM
    Number of beats counted in 10 seconds multiplied by 6      E.Walsh
*----------------------------------------------------------------------------*/
int CalculateBPM(int beats){
    return beats*6;
}
