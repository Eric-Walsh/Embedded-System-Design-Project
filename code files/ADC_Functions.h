#include "MKL25Z4.h"

void Init_ADC(void);

void ADC_IRQHandler(void);

int GetBeats(void);

void StartRecording(int fileNum);

void StopRecording(void);

void ResetThresh(void);
