#ifndef Audio_Sampling_H
#define Audio_Sampling_H
#include "AdvancedLED5.h"

//Interupt Service Routine for Collecting Audio samples at 40khz
//void IRAM_ATTR sampleInterrupt();

//Thread to perform a FFT on incoming audio samples. Sends the results to the LEDHandler for use
//in animations and analysis such as beat detection
void FFTLoop(void * param);

//Class for sending bandvalues to the ledhandler via queue
class bandVals{
public:
    int bandValues[16];
};



#endif