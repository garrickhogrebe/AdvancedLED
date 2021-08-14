#ifndef AL_Loader_H
#define AL_Loader_H
#include "AdvancedLED4.h"

//Variable loader class for loading animations
class Loader {
public:
	//How many variables are loaded into the array right now
	int num_loaded = 0;
	//Array for holding the potential variables to load. May change to not be array at some point
	int variables[1000];

	//Clears the Loader class to prepare for new load
	void clearLoader();

	//ToDo appends a variable to the loader class
	void append(int new_int);


};

extern Loader main_loader;












#endif