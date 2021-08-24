#ifndef AL_Animations_H
#define AL_Animations_H
#include "AdvancedLED4.h"
#include "Loader.h"
#include "LEDHandler.h"
#include "AudioSampling.h"
#include "Utilities.h"



//Animation Class. Contains the function which writes to leds as well as information about the animation such as how to load it and how many inputs it expects
class animation {
public:
	//How many inputs it needs to be created
	int num_inputs;
	//Does it follow standard loading, or is it custom
	bool standard_load;
	//the function it plays to update leds
	void (*play)(int start_location, class LEDHandler* handler);

	//function for called to return how many variables this animation requires to be loaded into the handler
	int determineNumVariables(Loader* loader);

	//the function it uses to be loaded if it is non-standard. Returns the number of variables the function will use
	void (*altLoad)(Loader* loader);
	//Name of this animation
	String name;
	//ToDo names and descriptions for each of the inputs

	//Pointers to next and previous animation. This is useful for keeping them all in a list
	class animation* next = NULL;
	class animation* prev = NULL;


	//ToDo constructor
	animation(int number_of_inputs, String animation_name, bool load_type, void (*play_function)(int, LEDHandler*), void (*load_function)(Loader*) = 0);

};

//Animation Class List. Currently a singleton containing all of the animations

class animation_list {
public:
	//First animation in the list
	class animation* start = NULL;
	//How many animations in the list
	int size = 0;
	
	//Appends a new animation to the list. Is called when an animation is constructed
	void append(class animation* new_animation);

	//Print out all of the animations
	void printAnimations();

};
extern animation_list main_animation_list;

//~~~~~~~~~~~~~~~~~~~~
void sinelon(int variable_start, LEDHandler* handler);
void sinelonLoad(Loader* loader);
void printBandValues(int variable_start, LEDHandler* handler);
void printPeaks(int variable_start, LEDHandler* handler);
void printAverages(int variable_start, LEDHandler* handler);
void peakLerp(int variable_start, LEDHandler* handler);
void peakLerpLoad(Loader* loader);
void peakLerpSpectrum(int variable_start, LEDHandler* handler);
void fadeBlock(int variable_start, LEDHandler* handler);
void fadeBlockLoad(Loader* loader);
void fillSolid(int variable_start, LEDHandler* handler);
void gravityBall(int variable_start, LEDHandler* hanlder);
void gravityBallLoad(Loader* loader);
void beatBar(int variable_start, LEDHandler* handler);
void beatBarLoad(Loader* loader);
void beatDetector(int variable_start, LEDHandler* handler);
void beatDetectorLoad(Loader* loader);
void zoomer(int variable_start, LEDHandler* handler);
void tetris(int variable_start, LEDHandler* handler);
void tetrisLoad(Loader* loader);
void musicColor(int variable_start, LEDHandler* handler);
void musicColorLoad(Loader* loader);
void musicColorVolume(int variable_start, LEDHandler* handler);
void musicColorVolumeLoad(Loader* loader);
void duelingBars(int variable_start, LEDHandler* handler);
void duelingBarSender(int variable_start, LEDHandler* handler);

#endif 