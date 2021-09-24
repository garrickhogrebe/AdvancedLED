#include "Utilities.h"

int easyLerp(int input_start, int input_end, int input, int output_start, int output_end) {
    int input_range = input_end - input_start ;
    if (input_range <= 0) {
        //Serial.println("bad lerp input");
        return 0;
    }
    int output_range = output_end - output_start;
    uint8_t f = (input - input_start) * 255 / (input_range);
    int val = output_start + (f * output_range) / 255;
    if (val > output_end) {
        Serial.println("Lerp failed");
    }
    //Serial.print("Lerped val: ");
    //Serial.println(val);
    return val;
}

uint8_t getFract8(int start, int end, int input) {
    int range;
    if (end > start) {
        range = end - start;
        return ((input - start) * 255) / range;
    }
    else if (start > end) {
        range = start - end;
        return ((start - input) * 255) / range;
    }
    else {
        Serial.println("bad getfract8, start = end");
        return 255;
    }
}