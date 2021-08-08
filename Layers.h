#ifndef AL_Layers_H
#define AL_Layers_H
#include "AdvancedLED4.h"

//Layer class contains the CRGB array as well as a few variables for the layer effect to use. When loading a new layer a layer effect is chosen
class layer {
public:
	CRGB layer_leds[NUM_LEDS];
	class layer_effect* effect;

};

//Layer_effect class is similar to animation class as each object has a unique 
class layer_effect {
public:
	//Function which modifies the CRGB array in the layer object that holds it
	void (*effect_function)(layer* my_layer);

	//Name of this layer effect
	String name;

	//Pointers to next and previous layer effects as they are stored in a list
	layer_effect* next = NULL;
	layer_effect* prev = NULL;

	//Constructor for this layer effect. Sets the effect function as well as its name and appends it to the layer_effect list
	layer_effect(String new_name, void (*func)(layer* my_layer));
};

//List for choosing layer affect. Similar to animation list
class layer_effect_list {
public:
	//size of the list
	int size = 0;
	//starting layer effect pointer
	layer_effect* start = NULL;
	//function to append a new layer_effect
	void append(layer_effect* new_layer_effect);
	//function to print all layer effects
	void printInfo();
};

extern layer_effect_list main_layer_effect_list;

//~~~~~~~~~~~~~~~~~~~~~~~
void layer_test(layer* my_layer);



















#endif