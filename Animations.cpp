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
	for (int i = 0; i < size; i++) {
		Serial.print(i);
		Serial.print(": ");
		Serial.println(current->name);
		current = current->next;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Animation Play Functions Below


void test(int variable_start, LEDHandler* handler) {
	int pos = beatsin16(10, 0, 160);
	handler->leds[pos] = CHSV(50, 255, 255);
}


animation Test1(5, "This is Test1", true, test);
animation Test2(5, "This is Test2", true, test);