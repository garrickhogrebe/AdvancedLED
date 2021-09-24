#include "AudioSampling.h"
#include <arduinoFFT.h>

static int16_t bufPos = 0;
static uint8_t sampleBuffer = 0;
static hw_timer_t *timer = NULL;
static double localSamples[2][SAMPLES];
static double localSamplesImaginary[2][SAMPLES];
QueueHandle_t audioDataQueue = xQueueCreate(1, sizeof(bandVals));


//Interrupt for sampling
void IRAM_ATTR sampleInterrupt(){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    //If current buffer full
    if(bufPos >= SAMPLES){
        //Serial.println(bufPos);
        //Check if other buffer ready (Via semaphore)
        if(xSemaphoreTakeFromISR(empty_buffers, NULL)){//If ready give semaphore to let FFT know, switch to new buffer, counter = 0
            //Serial.println("taken");
            if(!xSemaphoreGiveFromISR(full_buffers, &xHigherPriorityTaskWoken)){
                Serial.println("Couldn't give full buffer");//This should never happen but checking just in case
            }
            sampleBuffer = (sampleBuffer + 1)%2;
            bufPos = 0;
        }
        else{//If not ready return
            //Serial.println("all buffers full");
            return;
        }
    }
    //DO THE ACTUAL MEASUREMENT NOW OR YOUR FIRED
    localSamples[sampleBuffer][bufPos] = analogRead(AUDIO_IN_PIN);
    //Serial.println(localSamples[sampleBuffer][bufPos]);
    localSamplesImaginary[sampleBuffer][bufPos] = 0;
    bufPos++;

    //make scheduler do thing
    if (xHigherPriorityTaskWoken){
        portYIELD_FROM_ISR();
    }
}

void FFTLoop(void * param){
    uint8_t index = 0;//Indicates which data buffer we will be doing the FFT on
    bandVals newBandVals;
    arduinoFFT FFT[2] = {arduinoFFT(localSamples[0], localSamplesImaginary[0], SAMPLES, SAMPLING_FREQ), arduinoFFT(localSamples[1], localSamplesImaginary[1], SAMPLES, SAMPLING_FREQ)};
    //Set up the ADC timer interrupt on core 0
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &sampleInterrupt, true);
    timerAlarmWrite(timer, 25, true);
    timerAlarmEnable(timer);

    while(1){
        //Attempt to take a full buffer
        if(!xSemaphoreTake(full_buffers, 0)){//No buffers are ready to perform fft
            //Serial.println("No Buffers for fft");
            continue;
        }
        //We have succesfully taken the semaphore, Begin FFT
        FFT[index].DCRemoval();
        FFT[index].Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT[index].Compute(FFT_FORWARD);
        FFT[index].ComplexToMagnitude();
        for (int i = 0; i<NUM_BANDS; i++){
            newBandVals.bandValues[i] = 0;
        }
        for (int i = 2; i < (SAMPLES/2); i++){       // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency bin and its value the amplitude.
            if (localSamples[index][i] > NOISE) {                    // Add a crude noise filter

                //16 bands, 12kHz top band
                if (i<=2 )           newBandVals.bandValues[0]  += (int)localSamples[index][i];
                if (i>2   && i<=3  ) newBandVals.bandValues[1]  += (int)localSamples[index][i];
                if (i>3   && i<=5  ) newBandVals.bandValues[2]  += (int)localSamples[index][i];
                if (i>5   && i<=7  ) newBandVals.bandValues[3]  += (int)localSamples[index][i];
                if (i>7   && i<=9  ) newBandVals.bandValues[4]  += (int)localSamples[index][i];
                if (i>9   && i<=13 ) newBandVals.bandValues[5]  += (int)localSamples[index][i];
                if (i>13  && i<=18 ) newBandVals.bandValues[6]  += (int)localSamples[index][i];
                if (i>18  && i<=25 ) newBandVals.bandValues[7]  += (int)localSamples[index][i];
                if (i>25  && i<=36 ) newBandVals.bandValues[8]  += (int)localSamples[index][i];
                if (i>36  && i<=50 ) newBandVals.bandValues[9]  += (int)localSamples[index][i];
                if (i>50  && i<=69 ) newBandVals.bandValues[10] += (int)localSamples[index][i];
                if (i>69  && i<=97 ) newBandVals.bandValues[11] += (int)localSamples[index][i];
                if (i>97  && i<=135) newBandVals.bandValues[12] += (int)localSamples[index][i];
                if (i>135 && i<=189) newBandVals.bandValues[13] += (int)localSamples[index][i];
                if (i>189 && i<=264) newBandVals.bandValues[14] += (int)localSamples[index][i];
                if (i>264          ) newBandVals.bandValues[15] += (int)localSamples[index][i];
            }
        }
        //Serial.println(bandValues[0]);
        //Done with the FFT, give an empty_buffer
        if(!xSemaphoreGive(empty_buffers)){
            //This should never happen, but i've included it to test
            Serial.println("Could not give empty buff");
        }
        //Update the index so next FFT is on the next buffer
        index = (index + 1)%2;

        //add band values to queue
        if (xQueueSend(audioDataQueue, (void *)&newBandVals, 0) != pdTRUE){
            //Serial.println("Queue Full");
        }
    }
}