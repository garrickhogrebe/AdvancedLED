#ifndef Audio_Sampling_H
#define Audio_Sampling_H
#include "AdvancedLED4.h"

extern int gBandValues[NUM_BANDS];

//Task to run on core 0 which will sample audio data and perform FFT
void sampleTask(void* parameter);

class audioData {
public:
	int bandValues[NUM_BANDS];

	int prevBandValues[NUM_BANDS];

	int bandMultipliers[NUM_BANDS];

	int bandNoiseFilters[NUM_BANDS];

	int weightedAverages[NUM_BANDS];

	int weightedPeaks[NUM_BANDS];

	bool peakDetection[NUM_BANDS];

	bool bandsIncreasing[NUM_BANDS];

	bool bandsDecreasing[NUM_BANDS];

	void getNewData();

	//Play this function when no audio is being played to properly adjust the filters for each band
	void calibrateNoise();

	//Checks to see if each band has a new peak and slowly decays that peak over time
	void calcWeightedPeaks();

};

extern audioData mainAudioData;

#endif