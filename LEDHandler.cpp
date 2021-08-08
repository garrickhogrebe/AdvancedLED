#include "LEDHandler.h"

void LEDHandler::initializeHandler() {
	Serial.println("yeet");
	//initialize proper values for the handler's memory
	for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		animation_array[x] = NULL;
		variable_start_locations[x] = -1;
		number_of_variables[x] = 0;
		start_time[x] = 0;
		layer_index_array[x] = -1;
		dependencies[x] = -1;
		animations_to_delete[x] = false;
	}
	for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
		animation_variables[x] = 0;
		variable_in_use[x] = false;
	}
	for (int x = 0; x < MAX_LAYERS; x++) {
		//loaded_layers[x] = NULL;
	}
	handler_animation_list = &main_animation_list;
	handler_loader = &main_loader;
	handler_layer_effect_list = &main_layer_effect_list;
	current_time = millis();

	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
	

}

void LEDHandler::run() {
	//Serial.println("boot");
	audioUpdates();
	//Serial.println("boot1");
	current_time = millis();
	//Serial.println("boot2");
	playAnimations();
	//Serial.println("boot3");
	deleteMarkedAnimations();
	//Serial.println("boot4");
	//FastLED.show();

}

void LEDHandler::bluetoothCheck() {

}

void LEDHandler::audioUpdates() {

}

void LEDHandler::playAnimations() {
	for (animation_index_number = 0; animation_index_number < NUMBER_OF_ANIMATIONS; animation_index_number++) {
		if (animation_array[animation_index_number] != NULL) {
			//run the play function for this animation
			animation_array[animation_index_number]->play(variable_start_locations[animation_index_number], this);
		}
	}
}

void LEDHandler::deleteMarkedAnimations() {
	bool deleted = false;
	//Check each animation to see if it needs to be deleted. It can either be marked, or a dependency has been deleted
	for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		if (animations_to_delete[x] == true) {
			deleteAnimation(x);
			animations_to_delete[x] = false;
			deleted = true;
		}
		else if (animation_array[dependencies[x]] == NULL && dependencies[x] != -1) {
			deleteAnimation(x);
			deleted = true;
		}
	}
	//A dependency may of been deleted so run through again until no dependencies are deleted
	if (deleted) {
		Serial.println("MORE");
		deleteMarkedAnimations();
	}
	return;
}

void LEDHandler::deleteAnimation(int index){
	int num_variables = number_of_variables[index];
	int start_location = variable_start_locations[index];
	//mark variables as not in use
	for (int x = 0; x < num_variables; x++) {
		variable_in_use[start_location + x] = false;
	}
	//remove animation from the array
	animation_array[index] = NULL;
}

void LEDHandler::markForDeletion(int index) {
	animations_to_delete[index] = true;
}

void LEDHandler::addAnimation(animation* new_animation, int layer_index, Loader* loader) {
	int x;
	int animation_index;
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

	//determine number of variables needed
	int num_variables = new_animation->determineNumVariables(loader);
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
	number_of_variables[animation_index] = num_variables;
	//ToDo assign the proper layer
	layer_index_array[animation_index] = layer_index;

	//load variables 
	for (int i = 0; i < num_variables; i++) {
		variable_in_use[start + i] = true;
		animation_variables[start + i] = loader->variables[i];
	}


	
}

int LEDHandler::cleanVariableArray() {
	int reduce_amount = 0;
	int start = 0;
	int option = 0;//0 means looking for start of gap, 1 means looking for end of gap
	//locate gaps in the variable array
	for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
		if (option == 0) {
			if (variable_in_use[x] == false) {
				option = 1;
				//reduce all variable starts between start and x by reduce amount
				for (int i = 0; i < NUMBER_OF_ANIMATIONS; i++) {
					if (variable_start_locations[i] >= start && variable_start_locations[i] < x) {
						variable_start_locations[i] -= reduce_amount;
					}
				}
				start = x;
				reduce_amount++;
			}
		}
		else if (option == 1) {
			if (variable_in_use[x] == true) {
				//next option now looking for NULL
				option = 0;

			}
			else {//still empty
				reduce_amount++;
			}
		}
	}
	// Actually adjust the variable array
	start = 0; //repurposing start varaible to keep track of where variables should go
	for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
		if (variable_in_use[i] == true) {
			animation_variables[start] = animation_variables[i];
			variable_in_use[start] = true;
			start++;
		}
	}
	for (int x = start; x < NUMBER_OF_VARIABLES; x++) {
		variable_in_use[x] = false;
	}
	return start;
}

void LEDHandler::printInfo() {
	Serial.println("Printing Animations: ");
	for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
		if (animation_array[x] != NULL) {
			Serial.print(x);
			Serial.print(": ");
			Serial.print(animation_array[x]->name);
			Serial.print(", Dependency: ");
			Serial.print(dependencies[x]);
			Serial.print(", Number Variables: ");
			Serial.print(number_of_variables[x]);
			Serial.print(", Layer index: ");
			Serial.print(layer_index_array[x]);
			Serial.print(", Layer Effect: ");
			Serial.println(loaded_layers[layer_index_array[x]].effect->name);
		}

	}
	Serial.println("Printing Variables");
	for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
		if (variable_in_use[x] == true) {
			Serial.print(x);
			Serial.print(": ");
			Serial.println(animation_variables[x]);
		}
	}
	Serial.println("Potential layer effects");
	handler_layer_effect_list->printInfo();
}