#ifndef AdvancedLED4_H
#define AdvancedLED4_H
#include "Arduino.h"
#include<FastLED.h>


#define NUMBER_OF_ANIMATIONS 450
#define NUMBER_OF_VARIABLES NUMBER_OF_ANIMATIONS*10
#define MAX_LAYERS 20
#define NUM_LEDS 167
#define DEBUG

#define DATA_PIN    15
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS          140

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

FASTLED_USING_NAMESPACE

#endif