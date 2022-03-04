#include "MKL25Z4.h"

void Init_PIT0(unsigned period);

void Init_PIT1(unsigned period);

void Start_PIT(int channel);

void Stop_PIT(int channel);

void PIT_IRQHandler(void);
