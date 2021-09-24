#ifndef Utilities_H
#define Utilities_H
#include "AdvancedLED5.h"

//function for linear interpolation. Returns desired integer value for output
int easyLerp(int input_start, int input_end, int input, int output_start, int output_end);
#endif