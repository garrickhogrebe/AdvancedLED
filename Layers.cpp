#include "Layers.h"

layer_effect_list main_layer_effect_list;

layer_effect::layer_effect(String new_name, void (*func)(layer* my_layer)) {
	name = new_name;
	effect_function = func;
	main_layer_effect_list.append(this);
}

void layer_effect_list::append(layer_effect* new_layer_effect) {
	layer_effect* current = start;
	if (start == NULL) {//Append the first animation
		start = new_layer_effect;
		size++;
	}
	else {
		for (int i = 0; i < size - 1; i++) {//Go to end of list
			current = current->next;
		}
		//append the new animation
		current->next = new_layer_effect;
		new_layer_effect->prev = current;
		size++;
	}
}

void layer_effect_list::printInfo() {
	layer_effect* current = start;
	for (int i = 0; i < size; i++) {
		Serial.print(i);
		Serial.print(": ");
		Serial.println(current->name);
		current = current->next;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~
void layer_test(layer* my_layer) {

}