#ifndef AL_LEDHandler_H
#define AL_LEDHandler_H
#include "AdvancedLED4.h"
#include "Animations.h"
#include "Loader.h"
#include "AudioSampling.h"


//
//LED handler class. Highest level of abstraction
class LEDHandler {
public:
	//Bluetooth
	BluetoothSerial ESP_BT;

	//Array of LEDS
	CRGB leds[NUM_LEDS];

	//Array containing all currently loaded animations
	class animation* animation_array[NUMBER_OF_ANIMATIONS];

	//Array showing where to find each animations variables for the correspodning index
	int variable_start_locations[NUMBER_OF_ANIMATIONS];

	//Array containing how many variables each animation has
	int number_of_variables[NUMBER_OF_ANIMATIONS];

	//Array containing time values for each animation
	unsigned long long int start_time[NUMBER_OF_ANIMATIONS];

	//Array containing all of the variables for each animation
	int animation_variables[NUMBER_OF_VARIABLES];

	//Array specifying if the variables is in use or not
	bool variable_in_use[NUMBER_OF_VARIABLES];
	
	//Array which states which layer each animation is on
	int layer[NUMBER_OF_ANIMATIONS];
	
	//Sorted array which tells the handler which order to run the animations
	int play_order[NUMBER_OF_ANIMATIONS];

	//Array containing how to write each animation
	uint8_t write_type[NUMBER_OF_ANIMATIONS];

	//Old Layer Method
	//Array specifying which layer each animation writes too
	//int layer_index_array[NUMBER_OF_ANIMATIONS];
	//ToDo Array containing all currently loaded layers
	//class layer loaded_layers[MAX_LAYERS];
	//List of possible layer techniques to choose from
	//class layer_effect_list* handler_layer_effect_list;
	//end old layer method

	//Array Containing dependancies for deleting animations. A value of -1 indicates this animation is a root and independent
	int dependencies[NUMBER_OF_ANIMATIONS];

	//Array to tell that an animation has been marked for deletion
	bool animations_to_delete[NUMBER_OF_ANIMATIONS];

	//The loader for this handler
	class Loader* handler_loader;

	//List of Possible Animations to choose from
	class animation_list* handler_animation_list;

	//ToDo triggers to choose from
	//ToDo color palletes to choose from

	//Variable which holds the current time. Determined at the start of each run cycle
	unsigned long long int current_time;

	//Variable specifying the index of whatever animation is currently being played
	int animation_index_number;

	//Handlers audio data
	class audioData* handler_audio_data;

	//initializes the LEDHandler
	void initializeHandler();

	//runs a full cycle of what needs to be done by the handler each frame 
	void run();

	//ToDo checks for bluetooth message and does proper updates
	void bluetoothCheck();
	
	//ToDo updates audio information, calculates meta data and updates triggers
	void audioUpdates();

	//Plays each animation
	void playAnimations();

	//Deletes animations marked for deletion
	void deleteMarkedAnimations();

	//marks an animation for deletion
	void markForDeletion(int index);

	//deletes a single animation at the given index
	void deleteAnimation(int index);

	//ToDo (just layer part) Adds an animation to the handlers animation array. Need to update parameters
	void addAnimation(animation* new_animation, int layer_index, Loader* loader, uint8_t writeType = 1);

	//defragments the variable array, returns the first available memory location
	int cleanVariableArray();

	//Prints currently loaded animations and their layer. Prints variables
	void printInfo();

	//Sorts the animations by layers using merge sort
	void mergeSort(int arr[], int l, int r);

	//Merge function for merge sort
	void merge(int arr[], int l, int m, int r);

	//Load a new animation through the serial console
	void serialAddAnimation();

	//Delete an animation through the serial console
	void serialDeleteAnimation();

	//ToDo Edit an animation's layer through the serial console
	void serialEditLayer();

	//Get user instructions from the serial monitor
	void serialUpdates();

	//
	void writeLed(int pos, uint8_t writeType, CRGB color);

	void clearHandler();

	//ToDo Send message containing all animations to choose from
	//ToDo send message containing all layers to choose from
	//ToDo Send message containing all triggers to choose from
	//ToDo Send message containing all colorpalletes to choose from

	//ToDo Load new trigger
	//ToDo load new layer
	//ToDo load new colorPallete
};

#endif