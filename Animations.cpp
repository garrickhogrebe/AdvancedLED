#include "Animations.h"

animation_list main_animation_list;

animation::animation(int number_of_inputs, String animation_name, bool load_type, void (*play_function)(int, LEDHandler*), void (*load_function)(Loader*)) {
	num_inputs = number_of_inputs;
	name = animation_name;
	play = play_function;
	standard_load = load_type;
	altLoad = load_function;
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



animation FillSolid(4, "fill solid", true, fillSolid);
animation Sinelon(4, "Sinelon", false, sinelon, sinelonLoad);
animation PrintBandValues(0, "Print band values", true, printBandValues);
animation PrintPeaks(0, "Print band peaks", true, printPeaks);
animation PrintAverages(0, "Print band averages", true, printAverages);
animation PeakLerpObject(4, "lerp for band peak", false, peakLerp, peakLerpLoad);
animation PeakLerpSpectrum(4, "Lerp for all bands peak", true, peakLerpSpectrum);
animation FadeBlock(4, "Block that fades away", false, fadeBlock, fadeBlockLoad);
animation GravityBall(4, "gravity ball", false, gravityBall, gravityBallLoad);

void sinelon(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start+1];
	int color = handler->animation_variables[variable_start+2];
	int bpm = handler->animation_variables[variable_start+3];
	int* prev = &handler->animation_variables[variable_start + 4];
	int pos = beatsin16(bpm, start, end);
	int for_start;
	int for_end;
	if (*prev > pos) {
		for_start = pos;
		for_end = *prev;
	}
	else {
		for_start = *prev;
		for_end = pos;
	}
	for (int x = for_start; x <= for_end; x++) {
		handler->writeLed(x, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
	}
	*prev = pos;
	
	//handler->leds[pos] = CHSV(color, 255, 255);
}
void sinelonLoad(Loader* loader) {
	int start = loader->variables[0];
	int end = loader->variables[1];
	int bpm = loader->variables[3];
	loader->append(beatsin16(bpm, start, end));
}

void printBandValues(int variable_start, LEDHandler* handler) {
	for (int x = 0; x < NUM_BANDS; x++) {
		Serial.print("Band number ");
		Serial.print(x);
		Serial.print(": ");
		Serial.println(handler->handler_audio_data->bandValues[x]);
	}
}

void printPeaks(int variable_start, LEDHandler* handler) {
	for (int x = 0; x < NUM_BANDS; x++) {
		Serial.print("Peak Band number ");
		Serial.print(x);
		Serial.print(": ");
		Serial.println(handler->handler_audio_data->weightedPeaks[x]);
	}
}

void printAverages(int variable_start, LEDHandler* handler) {
	for (int x = 0; x < NUM_BANDS; x++) {
		Serial.print("Average, Band number ");
		Serial.print(x);
		Serial.print(": ");
		Serial.println(handler->handler_audio_data->weightedAverages[x]);
	}
}

void peakLerp(int variable_start, LEDHandler* handler) {//Lerp current band value in regards to max
	int band = handler->animation_variables[variable_start];
	int start = handler->animation_variables[variable_start+1];
	int end = handler->animation_variables[variable_start+2];
	int color = handler->animation_variables[variable_start+3];
	int* weighted_pos = &handler->animation_variables[variable_start + 4];
	int* prev = &handler->animation_variables[variable_start + 5];
	int mid = (start + end) / 2;
	int dead_space = 5;
	//int prev = *weighted_pos;
	class audioData* audio = handler->handler_audio_data;
	//Serial.println(audio->bandValues[band]);
	int pos = easyLerp(0, audio->weightedPeaks[band], audio->weightedAverages[band], start, end+dead_space);
	//handler->leds[pos] = CHSV(color, 255, 255);
	
	//int pos = easyLerp(0, audio->weightedPeaks[band], audio->bandValues[band], start, end+dead_space);
	*weighted_pos = ((*weighted_pos) * 50 + 50*pos) / 100;
	if (*weighted_pos == *prev) {
		*weighted_pos += 1;
	}
	pos = *weighted_pos - dead_space;
	if (pos < start) {
		return;
	}
	if (pos > end) {
		pos = end;
	}
	pos = pos / 2;
	//handler->leds[pos] = CHSV(color, 255, 255);
	handler->leds[mid + pos] += CHSV(color, 255, 255);
	handler->leds[mid - pos] += CHSV(color, 255, 255);
	*prev = *weighted_pos;
	//handler->leds[*weighted_pos] = CHSV(color, 255, 255);
}
void peakLerpLoad(Loader* loader) {
	loader->append(0);
	loader->append(0);
}

void peakLerpSpectrum(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start + 1];
	int color_base = handler->animation_variables[variable_start + 2];
	int color_shift = handler->animation_variables[variable_start + 3];
	Loader* loader = handler->handler_loader;
	for (int x = 0; x < NUM_BANDS; x++) {
		loader->clearLoader();
		loader->append(x);
		loader->append(start);
		loader->append(end);
		loader->append(color_base + x * color_shift);
		handler->addAnimation(&PeakLerpObject, x, loader, handler->write_type[handler->animation_index_number]);
	}
	handler->markForDeletion(handler->animation_index_number);
}

void fadeBlock(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start + 1];
	int color = handler->animation_variables[variable_start + 2];
	int fade_rate = handler->animation_variables[variable_start + 3];
	int *bright = &handler->animation_variables[variable_start + 4];

	for (int x = start; x <= end; x++) {
		handler->leds[x] = CHSV(color, 255, *bright);
	}
	//fade_rate = 255 - fade_rate;
	*bright = (*bright * (255 - fade_rate)) / 255;
	//*bright -= fade_rate;
	if (*bright <= 0) {
		handler->markForDeletion(handler->animation_index_number);
	}
}

void fadeBlockLoad(Loader* loader) {
	loader->append(255);
}


void fillSolid(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start + 1];
	int color = handler->animation_variables[variable_start + 2];
	int brightness = handler->animation_variables[variable_start + 3];
	for (int x = start; x <= end; x++) {
		handler->writeLed(x, handler->write_type[handler->animation_index_number], CHSV(color, 255, brightness));
		//handler->leds[x] += CHSV(color, 255, brightness);
	}
}

void gravityBall(int variable_start, LEDHandler* handler) {
	int start = handler->animation_variables[variable_start];
	int end = handler->animation_variables[variable_start + 1];
	int color = handler->animation_variables[variable_start + 2];
	int gravity = handler->animation_variables[variable_start + 3];
	int *pos = &handler->animation_variables[variable_start + 4];
	int *velocity = &handler->animation_variables[variable_start + 5];

	*pos = *pos + *velocity;
	*velocity = *velocity - gravity;
	if (*pos <= start) {
		*pos = start;
		handler->markForDeletion(handler->animation_index_number);
	}
	if (*pos > end) {
		handler->leds[end] = CHSV(color, 255, 255);
	}
	else {
		handler->leds[*pos] = CHSV(color, 255, 255);
	}
	//Serial.print("pos = ");
	//Serial.println(*pos);

}

void gravityBallLoad(Loader* loader) {
	int start = loader->variables[0];
	int end = loader->variables[1];
	int gravity = loader->variables[3];
	loader->append(start);
	//Change make bi directional
	loader->append(sqrt(2*gravity * (end - start)) - 1);
	loader->append(0);
}
