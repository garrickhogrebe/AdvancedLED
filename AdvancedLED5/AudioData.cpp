#include "AudioData.h"

audioData mainAudioData;


void audioData::audioUpdates() {
    //Reset values
    newAudioData = false;
    for(int i = 0; i < NUM_BANDS; i++){
        beat[i] = false;
        fluxHasDecreased[i] = false;
    }

    //Check for new audio data
    if (xQueueReceive(audioDataQueue, (void *)&bandValues, 0) == pdTRUE){
        newAudioData = true;
    }

    //Always perform these weighted averages even if no new data. Makes them smoother
    calcWeightedPeaks();

    if(!newAudioData){return;}

    //Only do these if there is new data
    //Total from each band
    total_current = 0;
    for(int i = 0; i < NUM_BANDS; i++){
        total_current += bandValues[i];
    }

    //Flux
    for(int i = 0; i < NUM_BANDS; i++){
        flux[i] = bandValues[i] - prevBandValues[i];
        if(flux[i] < 0){
            flux[i] = 0;
            averagePosFlux[i] = (95*averagePosFlux[i])/100;
        }else{
            averagePosFlux[i] = (averagePosFlux[i]*5 + 5*flux[i])/10;
        }
        if(flux[i] < prevFlux[i] || flux[i] == 0){
            fluxHasDecreased[i] = true;
        }
        //Weighted Averages
        if(flux[i] > fastDecayMaxFlux[i]){
            fastDecayMaxFlux[i] = flux[i];
            beat[i] = true;
            //Serial.println("0");
        }else fastDecayMaxFlux[i] = (fastDecayMaxFlux[i]*8)/10;

        if(flux[i] > slowDecayMaxFlux[i]){
            slowDecayMaxFlux[i] = flux[i];
        }else{
            if(flux[i] == 0){
                slowDecayMaxFlux[i] = slowDecayMaxFlux[i]*995/1000;
            }else{
                slowDecayMaxFlux[i] = (slowDecayMaxFlux[i]*999 + flux[i])/1000;
            }
        }

        //Beat?
        /*
        Serial.print("Flux: ");
        Serial.println(flux[i]);
        Serial.print("slowDecayMaxFlux[i]*3/8: ");
        Serial.println(slowDecayMaxFlux[i]*3/8);
        Serial.print("avgpos: ");
        Serial.println(averagePosFlux[i]);
        Serial.println(flux[i] < slowDecayMaxFlux[i]*3/8);
        Serial.println(flux[i] < averagePosFlux[i]);
        Serial.println(!fluxHasDecreased[i]);
         */
        if(flux[i] < slowDecayMaxFlux[i]*3/8 || flux[i] < averagePosFlux[i]){
            beat[i] = false;
            //Serial.println("1");
        }
        if(!fluxHasDecreased){
            beat[i] = false;
        }
        if(beat[i]){
            //Serial.println("beat");
            fluxHasDecreased[i] = false;
        }
        //Serial.println("end");
    }
    //Last thing - record prev
    for(int i = 0; i < NUM_BANDS; i++){
        prevFlux[i] = flux[i];
        prevBandValues[i] = bandValues[i];
    }
    /*
    Serial.print(flux[0]);
    Serial.print(",");
    Serial.print(fastDecayMaxFlux[0]);
    Serial.print(",");
    Serial.print(slowDecayMaxFlux[0]*3/8);
    Serial.print(",");
    Serial.println(averagePosFlux[0]);
    */
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
            weightedPeaks[x] = (weightedPeaks[x] * 99 + bandValues[x]) / 100;
        }
    }
}