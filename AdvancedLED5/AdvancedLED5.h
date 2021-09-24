#ifndef AdvancedLED5_H
#define AdvancedLED5_H
#include "Arduino.h"
#define FASTLED_INTERNAL
#include<FastLED.h>
#define FASTLED_INTERNAL
#include "BluetoothSerial.h"

//Fast LED Related
#define DATA_PIN1    26
#define DATA_PIN2    27
#define DATA_PIN3    14
#define DATA_PIN4    12

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS          90

//Audio Sampling Related
#define NUM_BANDS 16
#define SAMPLES 1024
#define SAMPLING_FREQ   40000
#define NOISE          5000
#define AUDIO_IN_PIN    34

//
#define oneStart 0
#define oneEnd 401
#define twoStart 402
#define twoEnd 779
#define threeStart 1
#define threeEnd 909
#define fourStart 1
#define fourEnd 1026

//LEDHandler engine related
#define NUMBER_OF_ANIMATIONS 500
#define NUMBER_OF_VARIABLES NUMBER_OF_ANIMATIONS*10
#define NUM_LEDS 1027























#define DEBUG
#define FRAMES_PER_SECOND  120


//Multithreading handles
extern SemaphoreHandle_t full_buffers;
extern SemaphoreHandle_t empty_buffers;
extern SemaphoreHandle_t ready_to_load;
extern SemaphoreHandle_t ready_to_display;
extern QueueHandle_t audioDataQueue;



#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

FASTLED_USING_NAMESPACE

#endif