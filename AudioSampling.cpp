#include "AudioSampling.h"
#include <arduinoFFT.h>

int gBandValues[NUM_BANDS];

int bandValues[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT = arduinoFFT(vReal, vImag, SAMPLES, SAMPLING_FREQ);
unsigned int sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));
unsigned long newTime;
bool new_audio_data = false;

audioData mainAudioData;

void sampleTask(void* parameter) {

    //Serial.println("1");
	for (;;) {
        //Sample the data
        for (int i = 0; i < NUM_BANDS; i++) {
            bandValues[i] = 0;
        }
        //Serial.println("2");
        // Sample the audio pin
        for (int i = 0; i < SAMPLES; i++) {
            newTime = micros();
            vReal[i] = analogRead(AUDIO_IN_PIN); // A conversion takes about 9.7uS on an ESP32
            vImag[i] = 0;
            //Serial.println("3");
            //yield();
            while ((micros() - newTime) < sampling_period_us) { /* chill */ }
        }
        //Serial.println("4");
        // Compute FFT
        FFT.DCRemoval();
        FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(FFT_FORWARD);
        FFT.ComplexToMagnitude();

        // Analyse FFT results
        for (int i = 2; i < (SAMPLES / 2); i++) {       // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
                        /*8 bands, 12kHz top band
              if (i<=3 )           bandValues[0]  += (int)vReal[i];
              if (i>3   && i<=6  ) bandValues[1]  += (int)vReal[i];
              if (i>6   && i<=13 ) bandValues[2]  += (int)vReal[i];
              if (i>13  && i<=27 ) bandValues[3]  += (int)vReal[i];
              if (i>27  && i<=55 ) bandValues[4]  += (int)vReal[i];
              if (i>55  && i<=112) bandValues[5]  += (int)vReal[i];
              if (i>112 && i<=229) bandValues[6]  += (int)vReal[i];
              if (i>229          ) bandValues[7]  += (int)vReal[i];*/

              //16 bands, 12kHz top band
            if (i <= 2 && vReal[i] > mainAudioData.bandNoiseFilters[0])           bandValues[0] += (int)vReal[i];
            if (i > 2 && i <= 3 && vReal[i] > mainAudioData.bandNoiseFilters[1]) bandValues[1] += (int)vReal[i];
            if (i > 3 && i <= 5 && vReal[i] > mainAudioData.bandNoiseFilters[2]) bandValues[2] += (int)vReal[i];
            if (i > 5 && i <= 7 && vReal[i] > mainAudioData.bandNoiseFilters[3]) bandValues[3] += (int)vReal[i];
            if (i > 7 && i <= 9 && vReal[i] > mainAudioData.bandNoiseFilters[4]) bandValues[4] += (int)vReal[i];
            if (i > 9 && i <= 13 && vReal[i] > mainAudioData.bandNoiseFilters[5]) bandValues[5] += (int)vReal[i];
            if (i > 13 && i <= 18 && vReal[i] > mainAudioData.bandNoiseFilters[6]) bandValues[6] += (int)vReal[i];
            if (i > 18 && i <= 25 && vReal[i] > mainAudioData.bandNoiseFilters[7]) bandValues[7] += (int)vReal[i];
            if (i > 25 && i <= 36 && vReal[i] > mainAudioData.bandNoiseFilters[8]) bandValues[8] += (int)vReal[i];
            if (i > 36 && i <= 50 && vReal[i] > mainAudioData.bandNoiseFilters[9]) bandValues[9] += (int)vReal[i];
            if (i > 50 && i <= 69 && vReal[i] > mainAudioData.bandNoiseFilters[10]) bandValues[10] += (int)vReal[i];
            if (i > 69 && i <= 97 && vReal[i] > mainAudioData.bandNoiseFilters[11]) bandValues[11] += (int)vReal[i];
            if (i > 97 && i <= 135 && vReal[i] > mainAudioData.bandNoiseFilters[12]) bandValues[12] += (int)vReal[i];
            if (i > 135 && i <= 189 && vReal[i] > mainAudioData.bandNoiseFilters[13]) bandValues[13] += (int)vReal[i];
            if (i > 189 && i <= 264 && vReal[i] > mainAudioData.bandNoiseFilters[14]) bandValues[14] += (int)vReal[i];
            if (i > 264 && vReal[i] > mainAudioData.bandNoiseFilters[15]) bandValues[15] += (int)vReal[i];
        }
        
        //ToDo Send the data to the handler
        //Take lock
        xSemaphoreTake(bandLock, portMAX_DELAY);
        //Copy and inform new measurements have arrived
        for (int x = 0; x < NUM_BANDS; x++) {
            gBandValues[x] = bandValues[x];
        }
        new_audio_data = true;
        //Give Lock
        xSemaphoreGive(bandLock);
        
     
	}
}

void audioData::getNewData() {
    //Take lock
    xSemaphoreTake(bandLock, portMAX_DELAY);
    //If no new measurements, leave
    if (new_audio_data == false) {
        xSemaphoreGive(bandLock);
        return;
    }//Otherwise copy the data
    for (int x = 0; x < NUM_BANDS; x++) {
        bandValues[x] = gBandValues[x];
    }
    new_audio_data = false;
    //RELEASE THE KRACKEN (lock)
    xSemaphoreGive(bandLock);
    calcWeightedPeaks();
}

void audioData::calibrateNoise() {
    int success = 0;

    //reset all noise filters to zero
    for (int i = 0; i < NUM_BANDS; i++) {
        bandNoiseFilters[i] = 0;
    }

    //Loop until all bands have been properly adjusted
    while (1) {
        //Take lock
        
        xSemaphoreTake(bandLock, portMAX_DELAY);
        if (new_audio_data) {
            success += 1;
            new_audio_data = false;
            for (int x = 0; x < NUM_BANDS; x++) {
                if (gBandValues[x]) {
                    success = 0;
                    bandNoiseFilters[x]+=100;
                }
            }
        }
        xSemaphoreGive(bandLock);
        //break if 100 success
        if (success > 50) {
            Serial.println("Noise Calibration Complete");
            for (int x = 0; x < NUM_BANDS; x++) {
                bandNoiseFilters[x] += 200;
                Serial.print("Band: ");
                Serial.print(x);
                Serial.print(", Noise: ");
                Serial.println(bandNoiseFilters[x]);
                
            }
            break;
        }
    }
}

void audioData::calcWeightedPeaks() {
    for (int x = 0; x < NUM_BANDS; x++) {
        if (bandValues[x] == 0) {//dont adjust if nothing is playing
            continue;
        }
        if (bandValues[x] > weightedPeaks[x]) {
            weightedPeaks[x] = bandValues[x];
        }
        else {
            weightedPeaks[x] = (weightedPeaks[x] * 999 + bandValues[x]) / 1000;
        }
    }
}