#ifndef Audio_Sampling_H
#define Audio_Sampling_H
#include "AdvancedLED4.h"

extern int gBandValues[NUM_BANDS];

//Task to run on core 0 which will sample audio data and perform FFT
void sampleTask(void* parameter);

#endif