#include "Animations.h"

animation_list main_animation_list;

animation::animation(int number_of_inputs, String animation_name, bool load_type, void (*play_function)(int, LEDHandler*)) {
	num_inputs = number_of_inputs;
	name = animation_name;
	play = play_function;
	standard_load = load_type;
	main_animation_list.append(this);
	
}

int animation::determineNumVariables(Loader* loader) {
	if (standard_load) {
		return num_inputs;
	}
	else {
		altLoad(loader);
		return loader->num_loaded;
	}
}

void animation_list::append(class animation* new_animation) {
	animation* current = start;
	if (start == NULL) {//Append the first animation
		start = new_animation;
		size++;
	}
	else {
		for (int i = 0; i < size - 1; i++) {//Go to end of list
			current = current->next;
		}
		//append the new animation
		current->next = new_animation;
		new_animation->prev = current;
		size++;
	}

}

void animation_list::printAnimations() {
	animation* current = start;
	Serial.println("All Possible Animations to choose from:");
	for (int i = 0; i < size; i++) {
		Serial.print(i);
		Serial.print(": ");
		Serial.println(current->name);
		current = current->next;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Animation Play Functions Below


animation Test(4, "This is Test", true, test);
animation Test1(0, "Print band values", true, test1);
animation Test2(3, "This is Test2", true, test2);

void test(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start+1];
	int color = handler->animation_variables[variable_start+2];
	int bpm = handler->animation_variables[variable_start+3];
	int pos = beatsin16(bpm, start, end);
	handler->leds[pos] = CHSV(color, 255, 255);
}

void test1(int variable_start, LEDHandler* handler) {
	for (int x = 0; x < NUM_BANDS; x++) {
		Serial.print("Band number ");
		Serial.print(x);
		Serial.print(": ");
		Serial.println(handler->handler_audio_data->bandValues[x]);
	}
}

void test2(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start + 1];
	int color = handler->animation_variables[variable_start + 2];
	for (int x = start; x <= end; x++) {
		handler->leds[x] = CHSV(color, 255, 255);
	}
}


