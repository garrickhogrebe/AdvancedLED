#ifndef AdvancedLED4_H
#define AdvancedLED4_H
#include "Arduino.h"
#define FASTLED_INTERNAL
#include<FastLED.h>

#define FASTLED_INTERNAL
#define NUMBER_OF_ANIMATIONS 450
#define NUMBER_OF_VARIABLES NUMBER_OF_ANIMATIONS*15
//#define MAX_LAYERS 5
#define NUM_LEDS 167
#define DEBUG

#define DATA_PIN    15
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS          140
#define NUM_BANDS 16
#define SAMPLES 1024
#define SAMPLING_FREQ   40000
#define AUDIO_IN_PIN    14
#define NOISE          9000

extern TaskHandle_t FFTstuff;
extern SemaphoreHandle_t bandLock;

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

FASTLED_USING_NAMESPACE

#endif