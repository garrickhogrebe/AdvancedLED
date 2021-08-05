#ifndef AL_Animations_H
#define AL_Animation_H
#include "AdvancedLED4.h"

//Animation Class. Contains the function which writes to leds as well as information about the animation such as how to load it and how many inputs it expects
class animation {
public:
	//How many inputs it needs to be created
	int num_inputs;
	//Does it follow standard loading, or is it custom
	bool standard_load;
	//the function it plays to update leds
	void (*play)(int);
	//the function it uses to be loaded if it is non-standard
	void (*load)();
	//Name of this animation
	String name;
	//ToDo names and descriptions for each of the inputs

	//Pointers to next and previous animation. This is useful for keeping them all in a list
	class animation* next;
	class animation* prev;


	//ToDo constructor
	animation();

};

//Animation Class List. Currently a singleton containing all of the animations

class animation_list {
public:
	//First animation in the list
	class animation* start;
	//How many animations in the list
	int size = 0;
	
	//ToDo Appends a new animation to the list. Is called when an animation is constructed
	void append(class animation* new_animation);

};






#endif