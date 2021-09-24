#include "Animations.h"

//ToDo: just add pointers for useful things like animation index number so i'm not using handler-> all the time
//ToDo: Also consider seperating animations into their own files
animation_list main_animation_list;

animation::animation(int number_of_inputs, String animation_name, bool load_type, void (*play_function)(int, LEDHandler*), void (*load_function)(Loader*, LEDHandler*)) {
    num_inputs = number_of_inputs;
    name = animation_name;
    play = play_function;
    standard_load = load_type;
    altLoad = load_function;
    main_animation_list.append(this);

}

int animation::determineNumVariables(Loader* loader, LEDHandler * handler) {
    if (standard_load) {
        return num_inputs;
    }
    else {
        altLoad(loader, handler);
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
animation BeatBar(5, "Beat Bar", false, beatBar, beatBarLoad);
animation BeatDector(4, "Beat Detector", false, beatDetector, beatDetectorLoad);
animation Zoomer(4, "Zoomer", true, zoomer);
animation Tetris(6, "Tetris", false, tetris, tetrisLoad);
animation MusicColor(4, "Music Color", false, musicColor, musicColorLoad);
animation MusicColorVolume(5, "Music Color Volume", false, musicColorVolume, musicColorVolumeLoad);
animation DuelingBars(8, "Deuling Bars", true, duelingBars);
animation DuelingBarSender(5, "Deuling Bar Sender", true, duelingBarSender);
animation BeatBar2(5, "BeatBar2", false, beatBar2, beatBar2Load);
animation BeatBar2trail(4, "BeatBar2Trail", true, beatBar2trail);
animation BeatBlocks(6, "Beat Blocks", false, lazyBeatBlocks, lazyBeatBlocksLoad);
animation IntArray(0, "Integer Array", false, intArray, intArrayLoad);
animation BeatZoom(5, "Beat Zoom", true, beatZoom);
animation BeatBar3(5,"Beat Bar 3", true, beatBar3);


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
void sinelonLoad(Loader* loader, LEDHandler* handler) {
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
        Serial.println(handler->hAD->flux[x]);
    }
}

void printPeaks(int variable_start, LEDHandler* handler) {
    for (int x = 0; x < NUM_BANDS; x++) {
        Serial.print("Peak Band number ");
        Serial.print(x);
        Serial.print(": ");
        Serial.println(handler->hAD->weightedPeaks[x]);
    }
}

void printAverages(int variable_start, LEDHandler* handler) {
    for (int x = 0; x < NUM_BANDS; x++) {
        Serial.print("Average, Band number ");
        Serial.print(x);
        Serial.print(": ");
        Serial.println(handler->hAD->weightedAverages[x]);
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
    class audioData* audio = handler->hAD;
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
void peakLerpLoad(Loader* loader, LEDHandler* handler) {
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
void fadeBlockLoad(Loader* loader, LEDHandler* handler) {
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

void gravityBallLoad(Loader* loader, LEDHandler* handler) {
    int start = loader->variables[0];
    int end = loader->variables[1];
    int gravity = loader->variables[3];
    loader->append(start);
    //Change make bi directional
    loader->append(sqrt(2*gravity * (end - start)) - 1);
    loader->append(0);
}

void beatBar(int variable_start, LEDHandler* handler) {
    int band = handler->animation_variables[variable_start];
    int start = handler->animation_variables[variable_start+1];
    int end = handler->animation_variables[variable_start+2];
    int color = handler->animation_variables[variable_start + 3];
    int locality = handler->animation_variables[variable_start+4];
    int *weighted_pos = &handler->animation_variables[variable_start+5];
    class audioData* audio = handler->hAD;


    //Lerp between current band value and it's peak value
    int pos = easyLerp(0, audio->total_peak, audio->total_current, start, end);
    *weighted_pos = (*weighted_pos * (100 - locality) + pos * locality) / 100;
    if (end > start) {
        for (int x = start; x < *weighted_pos; x++) {
            handler->writeLed(x, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
        }
    }
    //ToDo needs to work both directions

}
void beatBarLoad(Loader* loader, LEDHandler* handler) {
    if (loader->variables[4] > 100) {
        loader->variables[4] = 100;
    }
    else if (loader->variables[4] < 1) {
        loader->variables[4] = 1;
    }
    loader->append(0);
}

void beatDetector(int variable_start, LEDHandler* handler) {
    int band = handler->animation_variables[variable_start];
    int start = handler->animation_variables[variable_start + 1];
    int end = handler->animation_variables[variable_start + 2];
    int color = handler->animation_variables[variable_start + 3];
    int *state = &handler->animation_variables[variable_start + 4];
    int *prev_total = &handler->animation_variables[variable_start + 5];
    class audioData* audio = handler->hAD;

    //waiting for a decrease
    switch (*state) {
        case 0: if (audio->total_current < *prev_total) { //waiting for a decrease
                *state = 1;
            }
            break;
        case 1: if (audio->total_current > *prev_total) {// waiting for a rise
                *state = 0;
                handler->handler_loader->clearLoader();
                handler->handler_loader->append(start);
                handler->handler_loader->append(end);
                handler->handler_loader->append(color);
                handler->handler_loader->append(50);
                //handler->addAnimation(&Zoomer, handler->layer[handler->animation_index_number], handler->handler_loader, handler->write_type[handler->animation_index_number]);
                handler->addAnimation(&FadeBlock, handler->layer[handler->animation_index_number], handler->handler_loader, handler->write_type[handler->animation_index_number]);
                Serial.println("bruh");
            }
            break;
        default:
            *state = 0;
    }
    *prev_total = audio->total_current;

}
void beatDetectorLoad(Loader* loader, LEDHandler* handler) {
    loader->append(0);
    loader->append(0);
}

void zoomer(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start+1];
    int color = handler->animation_variables[variable_start+2];
    int period = handler->animation_variables[variable_start+3];
    int pos = start + ((handler->current_time - handler->start_time[handler->animation_index_number])/period);

    if (pos > end) {
        handler->markForDeletion(handler->animation_index_number);
        return;
    }

    handler->writeLed(pos, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
}

void tetris(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start + 1];
    int *base_color = &handler->animation_variables[variable_start + 2];
    int color_shift = handler->animation_variables[variable_start + 3];
    int period = handler->animation_variables[variable_start + 4];
    int bar_width = handler->animation_variables[variable_start + 5];
    int *num_set = &handler->animation_variables[variable_start + 6];
    int direction = 1;
    int max_bars = abs(start - end) / bar_width;


    //if all blocks are placed then strobe
    if (*num_set > max_bars) {
        *base_color += color_shift * (max_bars - 1);
        *num_set = 0;
    }
    int color = *base_color;

    //determin direction
    if (start > end) {
        direction = -1;
    }

    //fill in all set blocks
    int count = 0;
    for (int i = end; i != end - direction*bar_width*(*num_set); i -= direction) {
        if (count % bar_width == 0) {
            color = color + color_shift;
        }
        count++;
        handler->writeLed(i, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
    }
    color += color_shift;
    //determine location of moving block
    int pos = start + direction*((handler->current_time - handler->start_time[handler->animation_index_number]) / period);
    //if moving block has reached destination then increase number of set blocks to 1
    if (direction == 1) {
        if (pos >= end - bar_width * (*num_set)) {
            pos = end - bar_width * (*num_set) - 1;
            *num_set += 1;
            handler->start_time[handler->animation_index_number] = handler->current_time;
        }
    }
    else {
        if (pos <= end + bar_width * (*num_set)) {
            pos = end + bar_width * (*num_set) + 1;
            *num_set += 1;
            handler->start_time[handler->animation_index_number] = handler->current_time;
        }
    }
    //Serial.println(*num_set);
    for (int x = 0; x < bar_width; x++) {
        int y = pos - direction * x;
        if ((y > start && y > end) || (y < start && y < end)) {
            //*num_set = 0;
            //handler->start_time[handler->animation_index_number] = handler->current_time;
            return;
        }
        handler->writeLed(y, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
    }
}
void tetrisLoad(Loader* loader, LEDHandler* handler) {
    loader->variables[2] -= loader->variables[3];
    loader->append(0);
}

void musicColor(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start + 1];
    int color_start = handler->animation_variables[variable_start + 2];
    int color_end = handler->animation_variables[variable_start + 3];
    int* prev = &handler->animation_variables[variable_start + 4];
    audioData* audio = handler->hAD;


    //find the band with the highest weighted average
    uint8_t highest_index = 0;
    int highest_value = 0;
    for (int x = 0; x < NUM_BANDS; x++) {
        if (audio->weightedAverages[x] > audio->weightedAverages[highest_index]) {
            highest_index = x;
            highest_value = audio->weightedAverages[x];
        }
    }

    //add that band to the weighted average of whihc band is highest
    if (audio->total_current != 0) {
        fract8 f = highest_index * 255 / (NUM_BANDS - 1);
        *prev = (*prev * 235 + f * 20) / 255;
    }
    //changing use of highest index variable here
    //lerp the weighted averages to color
    highest_index = lerp8by8(color_start, color_end, *prev);

    //write the leds
    for (int i = start; i < end; i++) {
        handler->writeLed(i, handler->write_type[handler->animation_index_number], CHSV(highest_index, 255, 255));
    }

}
void musicColorLoad(Loader* loader, LEDHandler* handler) {
    loader->append(0);
}

void musicColorVolume(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start + 1];
    int color_start = handler->animation_variables[variable_start + 2];
    int color_end = handler->animation_variables[variable_start + 3];
    int locality = handler->animation_variables[variable_start + 4];
    int* prev = &handler->animation_variables[variable_start + 5];
    int* prev2 = &handler->animation_variables[variable_start + 6];
    int* prev3 = &handler->animation_variables[variable_start + 7];
    audioData* audio = handler->hAD;

    //find fraction current is of total
    uint8_t f = (audio->total_current+1) * 255 / (audio->total_peak+1);
    *prev = (*prev * (255-locality) + f * locality) / 255;
    *prev2 = (*prev2 * (255 - locality) + *prev * locality) / 255;
    *prev3 = (*prev3 * (255 - locality) + *prev2 * locality) / 255;

    //Serial.println(audio->total_current);
    //Serial.println(*prev);

    //lerp the weighted averages to color
    f = lerp8by8(color_start, color_end, *prev3);

    //write the leds
    for (int i = start; i < end; i++) {
        handler->writeLed(i, handler->write_type[handler->animation_index_number], CHSV(f, 255, 255));
    }

}
void musicColorVolumeLoad(Loader* loader, LEDHandler* handler) {
    loader->append(0);
    loader->append(0);
}

void duelingBars(int variable_start, LEDHandler* handler) {
    int upper_start = handler->animation_variables[variable_start];
    int upper_size = handler->animation_variables[variable_start + 1];
    int upper_color = handler->animation_variables[variable_start + 2];
    int lower_start = handler->animation_variables[variable_start + 3];
    int lower_size = handler->animation_variables[variable_start + 4];
    int lower_color = handler->animation_variables[variable_start + 5];
    int period = handler->animation_variables[variable_start + 6];
    int send_size = handler->animation_variables[variable_start + 7];
    int animation_index = handler->animation_index_number;

    Loader* loader = handler->handler_loader;

    //Every n milliseconds randomly choose  a side to send blocks to the otherside
    //Choice is weighted by how big each side is
    //If side would be empty dont send
    if (handler->current_time - handler->start_time[animation_index] >= period) {
        handler->start_time[animation_index] = handler->current_time;
        if (random16(upper_size + lower_size) < lower_size) {
            //lower side sends
            if (lower_size > send_size) {

                send_size = random8(1, send_size);
                loader->clearLoader();
                loader->append(animation_index);
                loader->append(lower_start + lower_size);
                loader->append(1);
                loader->append(5);
                loader->append(send_size);
                handler->addAnimation(&DuelingBarSender, handler->layer[animation_index] - 1, loader, handler->write_type[animation_index] );
                lower_size -= send_size;


            }
        }
        else {
            //upper side sends
            if (upper_size > send_size) {
                send_size = random8(1, send_size);
                loader->clearLoader();
                loader->append(animation_index);
                loader->append(upper_start - upper_size);
                loader->append(-1);
                loader->append(4);
                loader->append(send_size);
                handler->addAnimation(&DuelingBarSender, handler->layer[animation_index] - 1, loader, handler->write_type[animation_index]);
                upper_size -= send_size;
            }
        }
        handler->start_time[animation_index] = handler->current_time;
    }

    handler->animation_variables[variable_start + 1] = upper_size;
    handler->animation_variables[variable_start + 4] = lower_size;

    //write the leds for each side
    for (int x = 0; x < upper_size; x++) {
        handler->writeLed(upper_start - x, handler->write_type[animation_index], CHSV(upper_color, 255, 255));
    }
    for (int x = 0; x < lower_size; x++) {
        handler->writeLed(lower_start + x, handler->write_type[animation_index], CHSV(lower_color, 255, 255));
    }

}

void duelingBarSender(int variable_start, LEDHandler* handler) {
    //ToDo this should be dependant
    int destination_index = handler->animation_variables[variable_start];
    int start = handler->animation_variables[variable_start + 1];
    int direction = handler->animation_variables[variable_start + 2];
    int period = handler->animation_variables[variable_start + 3];
    int size = handler->animation_variables[variable_start + 4];

    //Find where the deuling bars stores its variables
    destination_index = handler->variable_start_locations[destination_index];
    //find new postion
    int pos = start + direction * ((handler->current_time - handler->start_time[handler->animation_index_number]) / period);

    if (direction == -1) {//destination boundaries are on the low side
        destination_index += 3;
    }
    int boundary1 = handler->animation_variables[destination_index];
    int boundary2 = handler->animation_variables[destination_index+1];
    boundary2 = boundary1 - direction * boundary2;

    //if new position lies within the destination, increase the size of the destination and delete this
    //Serial.print("pos: ");
    //Serial.print(pos);
    //Serial.print(" boundary 1: ");
    //Serial.print(boundary1);
    //Serial.print(" boundary2: ");
    //Serial.println(boundary2);
    if ((pos <= boundary1 && pos >= boundary2) || (pos >= boundary1 && pos <= boundary2)) {
        handler->animation_variables[destination_index+1] += size;
        handler->markForDeletion(handler->animation_index_number);
        //Serial.println("Deleted");
        return;
    }
    //otherwise write the leds
    int color = 0;
    if (direction == -1) {
        color = handler->animation_variables[destination_index - 1];
    }
    else {
        color = handler->animation_variables[destination_index + 5];
    }

    for (int x = 0; x < size; x++) {
        handler->writeLed(pos - direction*x, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
    }
}

void beatBar2(int variable_start, LEDHandler* handler) {
    int band = handler->animation_variables[variable_start];
    int start = handler->animation_variables[variable_start+1];
    int end = handler->animation_variables[variable_start+2];
    int locality = handler->animation_variables[variable_start+3];
    int color = handler->animation_variables[variable_start+4];
    int *target = &handler->animation_variables[variable_start+5];
    int *current = &handler->animation_variables[variable_start+6];
    int *prev = &handler->animation_variables[variable_start+7];
    audioData* audio = handler->hAD;
    int direction = 1;
    int maxSize;
    int bandVal;
    int bandValPeak;

    if (band >= 0 && band < 16) {
        bandVal = audio->bandValues[band];
        bandValPeak = audio->weightedPeaks[band];
    }
    else {

        bandVal = audio->total_current;
        bandValPeak = audio->total_peak;
    }

    if (start > end) {
        direction = -1;
        maxSize = start - end + 1;
    }
    else {
        maxSize = end - start + 1;
    }
    //weighted average of target
    *target = (*target * (100 - locality) + bandVal * locality) / 100;

    //weighted average of of current to target
    *current = (*current * (100 - locality) + *target * locality) / 100;

    //lerp between total
    int pos = easyLerp(0, bandValPeak, *current, 0, maxSize);

    if (*current > bandValPeak) {
        pos = maxSize;
    }

    /*
    if (pos > *peak) {
        *peak = pos;
    }

    int dif = *peak - pos;
    int count = 0;
    int bright;
    */

    if (direction == 1) {
        for (int x = 0; x < pos; x++) {
            handler->writeLed(start + x, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
        }
        /*
        for (int x = start + pos; x < start + *peak; x++) {
            bright = 255 - easyLerp(0, dif, count, 0, 255);
            count++;
            handler->writeLed(x, handler->write_type[handler->animation_index_number], CHSV(color, 255, bright));
        }
        */
    }
    else {
        for (int x = 0; x < pos; x++) {
            handler->writeLed(start - x, handler->write_type[handler->animation_index_number], CHSV(color, 255, 255));
        }
        /*
        for (int x = start - pos; x > start - *peak; x--) {
            bright = 255 - easyLerp(0, dif, count, 0, 255);
            count++;
            handler->writeLed(x, handler->write_type[handler->animation_index_number], CHSV(color, 255, bright));
        }
        */
    }
    Loader* loader = handler->handler_loader;
    loader->clearLoader();
    if (direction == 1) {
        if (*prev > pos) {
            loader->append(handler->animation_index_number);
            loader->append(start + *prev);
            loader->append(start + pos);
            loader->append(1);
            //add the animation
            handler->addAnimation(&BeatBar2trail, handler->layer[handler->animation_index_number] + 1, loader, handler->write_type[handler->animation_index_number]);

        }

    }
    else {
        if (*prev > pos) {
            loader->append(handler->animation_index_number);
            loader->append(start - *prev);
            loader->append(start - pos);
            loader->append(1);
            handler->addAnimation(&BeatBar2trail, handler->layer[handler->animation_index_number] + 1, loader, handler->write_type[handler->animation_index_number]);
        }
    }

    //*peak = *peak - maxSize/10;
    *prev = pos;
}
void beatBar2Load(Loader* loader, LEDHandler* handler) {
    //locality between 0 and 100
    if (loader->variables[3] > 100) {
        loader->variables[3] = 100;
    }
    if (loader->variables[3] < 0) {
        loader->variables[3] = 0;
    }
    loader->append(0);
    loader->append(0);
    loader->append(0);
}

void beatBar2trail(int variable_start, LEDHandler* handler) {
    int parentLocation = handler->animation_variables[variable_start];
    parentLocation = handler->variable_start_locations[parentLocation];
    int *farEnd = &handler->animation_variables[variable_start+1];
    int *closeEnd = &handler->animation_variables[variable_start+2];
    int* cycles = &handler->animation_variables[variable_start + 3];
    int parentStart = handler->animation_variables[parentLocation + 1];
    int parentEnd = handler->animation_variables[parentLocation + 2];
    int hue = handler->animation_variables[parentLocation + 4];
    int parentPos = handler->animation_variables[parentLocation + 7];
    int direction = 1;
    int maxSize;
    int fadeRate;
    CRGB color;

    //determine direction
    if (*farEnd < *closeEnd) {
        direction = -1;
        parentPos = parentStart - parentPos;
        maxSize = parentStart - parentEnd;
    }
    else {
        parentPos = parentStart + parentPos;
    }

    //if the bar has passed the far end, delete this and return

    //BUGGY maybe
    if (abs(parentPos - parentStart) > abs(*farEnd - parentStart)) {
        //Serial.println("deletion 1");
        handler->markForDeletion(handler->animation_index_number);
        return;
    }

    //BUGGY MAYBE
    if (abs(parentPos - parentStart) > abs(*closeEnd - parentStart)) {
        *closeEnd = parentPos;
    }

    //each led has a fade rate which is based on a lerp of parents distance
    if (direction == 1) {
        for (int x = *closeEnd; x <= *farEnd; x++) {
            //determine fade rate
            fadeRate = easyLerp(parentStart, parentEnd, x, 10, 100);
            color = CHSV(hue, 255, 255);
            for (int y = 0; y < *cycles; y++) {
                color.fadeToBlackBy(fadeRate);
            }
            if (color.getAverageLight() == 0) {
                *farEnd = x;
                break;
            }
            handler->writeLed(x, handler->write_type[handler->animation_index_number], color);

        }

    }
    else {
        for (int x = *closeEnd; x >= *farEnd; x--) {
            fadeRate = easyLerp(0, maxSize, parentStart - x, 10, 100);
            color = CHSV(hue, 255, 255);
            for (int y = 0; y < *cycles; y++) {
                color.fadeToBlackBy(fadeRate);
            }
            if (color.getAverageLight() == 0) {
                *farEnd = x;
                break;
            }
            handler->writeLed(x, handler->write_type[handler->animation_index_number], color);
        }

    }

    //if the far end == close end, delete this
    /*
    Serial.print("far: ");
    Serial.print(*farEnd);
    Serial.print(" close: ");
    Serial.println(*closeEnd);
    */

    if (*farEnd == *closeEnd) {
        //Serial.println("deletion 2");
        handler->markForDeletion(handler->animation_index_number);
    }

    *cycles += 1;
}

void beatBlocks(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start + 1];
    int num_blocks = handler->animation_variables[variable_start + 2];
    int base_color = handler->animation_variables[variable_start + 3];
    int hit_color = handler->animation_variables[variable_start + 4];
    int band = handler->animation_variables[variable_start + 5];
    int mem_location = handler->animation_variables[variable_start + 6];
    int *prev_block = &handler->animation_variables[variable_start + 7];
    int block_to_toggle = 0;
    int variable_index = handler->variable_start_locations[mem_location];
    int block_size = (end - start) / num_blocks;
    CRGB color;

    //Cause colors to update
    for (int i = 0; i < num_blocks; i++) {
        if (handler->animation_variables[variable_index + i] < 255) {
            handler->animation_variables[variable_index + i] += 4;
            if(handler->animation_variables[variable_index + i] > 255){
                handler->animation_variables[variable_index + i] = 255;
            }
        }
    }

    //If beat toggle set a block to hit color
    if (handler->hAD->beat[band]) {
        //Serial.println("beat");
        block_to_toggle = random(0, num_blocks);
        if (block_to_toggle == *prev_block) {
            block_to_toggle = (block_to_toggle + 1) % num_blocks;
        }
        /*
        Serial.print("block toggling: " );
        Serial.println(block_to_toggle);
        Serial.print("Location: ");
        Serial.println(variable_index + block_to_toggle);
        Serial.print("Num blocks: ");
        Serial.println(num_blocks);
         */
        handler->animation_variables[variable_index + block_to_toggle] = 0;
        *prev_block = block_to_toggle;
    }

    //Write all the leds for each block blending between colors
    for (int i = 0; i < num_blocks; i++) {
        for (int j = 0; j < block_size; j++) {
            //Serial.println(start + i * block_size + j);
            color = blend(CHSV(hit_color, 255, 255), CHSV(base_color, 255, 255), handler->animation_variables[variable_index + i]);
            //color = CHSV(90, 255, 255);
            handler->writeLed(start + i * block_size + j, handler->write_type[handler->animation_index_number], color);
        }
    }
}

void beatBlocksLoad(Loader* loader, LEDHandler* handler){
    Loader loader2;
    loader2.clearLoader();
    for(int x = 0; x < loader->variables[2]; x++){
        loader2.append(0);
    }
    int mem_location = handler->addAnimation(&IntArray, 0, &loader2, 0);
    loader->append(mem_location);
    loader->append(255);
}

void intArray(int variable_start, LEDHandler* handler){
    //chill bro nothing to do here
    int neg = 0;
    for(int x = 0; x < 20; x++){
        //Serial.println(handler->animation_variables[variable_start + x]);
    }
}
void intArrayLoad(Loader* loader, LEDHandler* handler){
    //Do nothing again
}

void beatZoom(int variable_start, LEDHandler* handler){
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start + 1];
    int band = handler->animation_variables[variable_start+2];
    int color = handler->animation_variables[variable_start+3];
    int period = handler->animation_variables[variable_start+4];
    Loader * loader = handler->handler_loader;
    if(handler->hAD->beat[band]){
        loader->clearLoader();
        loader->append(start);
        loader->append(end);
        loader->append(color);
        loader->append(period);
        handler->addAnimation(&Zoomer,  handler->layer[handler->animation_index_number], loader, handler->write_type[handler->animation_index_number]);
    }
}

void lazyBeatBlocks(int variable_start, LEDHandler* handler) {
    int start = handler->animation_variables[variable_start];
    int num_toggle = handler->animation_variables[variable_start + 1];
    int end = handler->animation_variables[variable_start + 1];
    int num_blocks = 12;
    int base_color = handler->animation_variables[variable_start + 3];
    int hit_color = handler->animation_variables[variable_start + 4];
    int band = handler->animation_variables[variable_start + 5];
    int mem_location = handler->animation_variables[variable_start + 6];
    int *prev_block = &handler->animation_variables[variable_start + 7];
    int block_to_toggle = 0;
    int variable_index = handler->variable_start_locations[mem_location];
    int block_size = (end - start) / num_blocks;
    CRGB color;

    //Cause colors to update
    for (int i = 0; i < num_blocks; i++) {
        if (handler->animation_variables[variable_index + i] < 255) {
            handler->animation_variables[variable_index + i] += 4;
            if(handler->animation_variables[variable_index + i] > 255){
                handler->animation_variables[variable_index + i] = 255;
            }
        }
    }

    //If beat toggle set a block to hit color
    if (handler->hAD->beat[band]) {
        //Serial.println("beat");
        for (int x = 0; x < num_toggle; x++) {
            block_to_toggle = random(0, num_blocks);
            if (block_to_toggle == *prev_block) {
                block_to_toggle = (block_to_toggle + 1) % num_blocks;
            }
            /*
            Serial.print("block toggling: " );
            Serial.println(block_to_toggle);
            Serial.print("Location: ");
            Serial.println(variable_index + block_to_toggle);
            Serial.print("Num blocks: ");
            Serial.println(num_blocks);
             */
            handler->animation_variables[variable_index + block_to_toggle] = 0;
            *prev_block = block_to_toggle;
        }
    }

    //Write all the leds for each block blending between colors
    for (int i = 0; i < num_blocks; i++) {
        color = blend(CHSV(hit_color, 255, 255), CHSV(base_color, 255, 255), handler->animation_variables[variable_index + i]);
        if(i == 0){
            for (int j = 0; j <= 130; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 1){
            for (int j = 130; j <= 261; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 2){
            for (int j = 560; j <= 669; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 3){
            for (int j = 670; j <= 779; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 4){
            for (int j = 261; j <= 335; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 5){
            for (int j = 336; j <= 409; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 6){
            for (int j = 409; j <= 484; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 7){
            for (int j = 485; j <= 559; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 8){
            for (int j = 809; j <= 859; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 9){
            for (int j = 860; j <= 909; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 10){
            for (int j = 926; j <= 976; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }
        if(i == 11){
            for (int j = 976; j <= 1026; j++) {
                handler->writeLed(j, handler->write_type[handler->animation_index_number], color);
            }
        }

    }
}

void lazyBeatBlocksLoad(Loader* loader, LEDHandler* handler){
    Loader loader2;
    loader2.clearLoader();
    for(int x = 0; x < 12; x++){
        loader2.append(0);
    }
    int mem_location = handler->addAnimation(&IntArray, 0, &loader2, 0);
    loader->append(mem_location);
    loader->append(255);
}

void beatBar3(int variable_start, LEDHandler* handler){
    int start = handler->animation_variables[variable_start];
    int end = handler->animation_variables[variable_start+1];
    int band = handler->animation_variables[variable_start+2];
    CRGB color = CHSV(handler->animation_variables[variable_start+3], 255, 255);
    int *top = &handler->animation_variables[variable_start+4];
    int len;
    int direction = 1;
    audioData *audio = handler->hAD;

    if (start > end){
        direction = -1;
        len = start - end + 1;
    }else{
        len = end - start + 1;
    }
    //Serial.println(direction);
    /*
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!");
    Serial.println(start);
    Serial.println(end);
    Serial.println(len);
    Serial.println(audio->flux[band]);
     */
    int pos = easyLerp(0, audio->slowDecayMaxFlux[band], audio->flux[band], 0, len );

    *top = (*top * 91)/100;

    if (pos > *top){
        *top = pos;
    }


    if(direction == 1){
        for(int x = 0; x < *top; x++){
            handler->writeLed(start + x, handler->write_type[handler->animation_index_number], color);
        }
    }else{
        //Serial.println("REEEEE");
        //Serial.println(end);
        for(int x = 0; x < *top; x++){
            handler->writeLed(start - x, handler->write_type[handler->animation_index_number], color);
        }
    }

}