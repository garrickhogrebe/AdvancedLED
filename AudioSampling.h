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

	int bandNoiseFilters[NUM_BANDS] = { 3300, 3400, 3600, 3400, 3300, 3600, 3500, 3500, 3600, 3700, 3600, 3900, 3900, 4100, 4200, 4000 };

	int weightedAverages[NUM_BANDS];

	int weightedPeaks[NUM_BANDS];

	bool peakOccuring[NUM_BANDS];

	bool bandsIncreasing[NUM_BANDS];

	bool bandsDecreasing[NUM_BANDS];
	
	bool rising[NUM_BANDS];

	bool decreasing[NUM_BANDS];

	//8bit fraction of how much each band is present compared to its peak
	uint8_t fractPeak[NUM_BANDS];

	void getNewData();

	//Play this function when no audio is being played to properly adjust the filters for each band
	void calibrateNoise(CRGB arr[]);

	//Checks to see if each band has a new peak and slowly decays that peak over time
	void calcWeightedPeaks();

	//Calculates the values for fractPeak
	void calcFractPeak();

	//Calculates the weighted  averages
	void calcWeightedAverages();
};

extern audioData mainAudioData;

#endif