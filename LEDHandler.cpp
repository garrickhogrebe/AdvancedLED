#include "LEDHandler.h"

void LEDHandler::initializeHandler() {
	//initialize proper values for the handler's memory
	for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		animation_array[x] = NULL;
		variable_start_locations[x] = -1;
		number_of_variables[x] = 0;
		start_time[x] = 0;
		layer_array[x] = NULL;
		dependencies[x] = -1;
		animations_to_delete[x] = false;
	}
	for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
		animation_variables[x] = 0;
		variable_in_use[x] = false;
	}
	for (int x = 0; x < MAX_LAYERS; x++) {
		loaded_layers[x] = NULL;
	}
	current_time = millis();
}

void LEDHandler::run() {
	audioUpdates();
	current_time = millis();
	playAnimations();
	deleteAnimations();

}

void LEDHandler::bluetoothCheck() {

}

void LEDHandler::audioUpdates() {

}

void LEDHandler::playAnimations() {
	for (animation_index_number = 0; animation_index_number < NUMBER_OF_ANIMATIONS; animation_index_number++) {
		if (animation_array[animation_index_number] != NULL) {
			//ToDo run the play function for this animation
		}
	}
}

void LEDHandler::deleteAnimations() {
	bool deleted = false;
	//Check each animation to see if it needs to be deleted. It can either be marked, or a dependency has been deleted
	for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		if (animations_to_delete[x] == true) {
			//ToDo delete this animation
			animations_to_delete[x] = false;
			deleted = true;
		}
		else if (animation_array[dependencies[x]] == NULL) {
			//ToDo delete this animation
			deleted = true;
		}
	}
	//A dependency may of been deleted so run through again until no dependencies are deleted
	if (deleted) {
		deleteAnimations();
	}
	return;
}

void LEDHandler::markForDeletion(int index) {
	animations_to_delete[index] = true;
}

void LEDHandler::addAnimation(animation* new_animation) {
	int x;
	int animation_index;
	int num_variables = new_animation->num_inputs;
	int empty_in_row = 0;
	int full = 1;
	int start = 0;

	//Find the first available spot in the animation array
	for (x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		if (animation_array[x] == NULL) {
			animation_index = x;
			break;
		}
	}
	if (x == NUMBER_OF_ANIMATIONS) {
		#ifdef DEBUG
		Serial.println("Animation array is full. Animation not added");
		#endif
		return;
	}

	//TODO determine number of variables needed

	//Attempt to find a gap large enough to store all variables. If no gap is found defragment the variable memory, and check to see if it fits now
	for (x = 0; x < NUMBER_OF_VARIABLES; x++) {
		if (variable_in_use[x] == true) {
			empty_in_row = 0;
		}
		else {
			empty_in_row++;
		}
		if (num_variables == empty_in_row) {
			full = 0;
			break;
		}
	}
	if (full == 1) {
		start = cleanVariableArray();
		if (start + num_variables > NUMBER_OF_VARIABLES) {
			#ifdef DEBUG
			Serial.println("Variable Array is full. Animation not added");
			#endif
			return;
		}
	}
	else {
		start = x - num_variables + 1;
	}
	animation_array[animation_index] = new_animation;
	variable_start_locations[animation_index] = start;
	start_time[animation_index] = millis();
	//TODO assign the proper layer
	//layer_array[animation_index] = ;

	//TODO load variables either standard or non standard way


	
}

int LEDHandler::cleanVariableArray() {

}