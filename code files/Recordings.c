#include "MKL25Z4.h"
#include "gpio_defs.h"
#include "DAC_Functions.h"
#include "LEDs.h"

//The arrays that store the values for the waves
uint32_t waveArray1[1000];
uint32_t waveArray2[1000];

//saves the wave in array 1
void recordFile1(uint32_t data, int count){
    waveArray1[count] = data;
}

//saves the wave in array 2
void recordFile2(uint32_t data, int count){
    waveArray2[count] = data;
}

//Sends the contents of the chosen array to the DAC
void sendToDAC(int file, int startStopFlag){
	int loopCount = 0;
    for(int i = 0; i < 1000; i++){
        if(file == 1){
            DAC0->DAT[0].DATH = DAC_DATH_DATA1(waveArray1[i] >> 8);
            DAC0->DAT[0].DATL = DAC_DATL_DATA0(waveArray1[i]);
        } else if (file == 2){
            DAC0->DAT[0].DATH = DAC_DATH_DATA1(waveArray2[i] >> 8);
            DAC0->DAT[0].DATL = DAC_DATL_DATA0(waveArray2[i]);
        }
				if(startStopFlag == 1 && i == 999 && loopCount < 1000){
					i = 0;
					loopCount++;
				}
    }
}
