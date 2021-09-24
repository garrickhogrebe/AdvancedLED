#ifndef AL_AUDIO_H
#define AL_AUDIO_H
#include "AdvancedLED5.h"

class audioData{
public:
    bool newAudioData;
    int bandValues[NUM_BANDS];
    int prevBandValues[NUM_BANDS];
    int flux[NUM_BANDS];
    int prevFlux[NUM_BANDS];
    int slowDecayPeak[NUM_BANDS];
    int fastDecayPeak[NUM_BANDS];
    int slowDecayMaxFlux[NUM_BANDS];
    int fastDecayMaxFlux[NUM_BANDS];
    int averagePosFlux[NUM_BANDS];
    bool beat[NUM_BANDS];
    bool fluxHasDecreased[NUM_BANDS];

    //Old things still used by some animations
    int total_current;
    int total_peak;
    int weightedAverages[NUM_BANDS];
    int weightedPeaks[NUM_BANDS];
    void calcWeightedPeaks();

    void audioUpdates();
};

extern audioData mainAudioData;
#endif