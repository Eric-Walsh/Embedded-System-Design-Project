#include <MKL25Z4.h>

void init_SampleRate(void);

void ChangeSampleRate(int change);

void ReferenceWave(uint32_t value, int count);

uint32_t FindMax(void);

uint32_t FindMin(void);

uint32_t FindLowThresh(uint32_t magnitude, uint32_t min);

uint32_t FindHighThresh(uint32_t magnitude, uint32_t min);

int CalculateBPM(int beats);

