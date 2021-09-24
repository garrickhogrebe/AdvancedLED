#include "LEDHandler.h"

void LEDHandler::initializeHandler() {
    //Start bluetooth
    ESP_BT.begin("just test"); //Name of your Bluetooth Signal
    //Initialize proper values for the handler's memory
    pinMode(AUDIO_IN_PIN,INPUT_PULLUP);
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        animation_array[x] = NULL;
        variable_start_locations[x] = -1;
        number_of_variables[x] = 0;
        start_time[x] = 0;
        layer[x] = 0;
        dependencies[x] = -1;
        animations_to_delete[x] = false;
    }
    for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
        animation_variables[x] = 0;
        variable_in_use[x] = false;
    }
    handler_animation_list = &main_animation_list;
    handler_loader = &main_loader;
    hAD = &mainAudioData;
    current_time = millis();

    //Setup leds with fast led
    //ToDo: Non hardcoded solution for setting up multiple strips

    FastLED.addLeds<LED_TYPE, DATA_PIN1, COLOR_ORDER>(ledsOut1, 402).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN2, COLOR_ORDER>(ledsOut2, 378).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN3, COLOR_ORDER>(ledsOut3, 131).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN4, COLOR_ORDER>(ledsOut4, 117).setCorrection(TypicalLEDStrip);

    /*
    FastLED.addLeds<LED_TYPE, DATA_PIN1, COLOR_ORDER>(ledsOut1, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN2, COLOR_ORDER>(ledsOut2, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN3, COLOR_ORDER>(ledsOut3, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DATA_PIN4, COLOR_ORDER>(ledsOut4, NUM_LEDS).setCorrection(TypicalLEDStrip);
    */

    FastLED.setBrightness(BRIGHTNESS);
}

//ToDo: Thread version
void LEDHandler::run() {
    bandVals bandValues;
    bluetoothCheck();
    current_time = millis();
    hAD->audioUpdates();
    //Serial.println("0");
    playAnimations();
    //Take for semaphore
    //Serial.println("1");
    xSemaphoreTake(ready_to_load, portMAX_DELAY);
    //Serial.println("2");
    //load into out arrays

    /*
    for(int x = 0; x < NUM_LEDS; x++){
        ledsOut1[x] = leds[x];
        ledsOut2[x] = leds[x];
        ledsOut3[x] = leds[x];
        ledsOut4[x] = leds[x];
    }
     */



    for(int x = 0; x < NUM_LEDS; x++){

        //0 - 290 invert and out 1
        if( x >= 0 && x <= 401){
            ledsOut1[401 - x] = leds[x];
        }

        //291 - 597 out 2
        if( x >= 402 && x <= 779){
            ledsOut2[x - 402] = leds[x];
        }


        //598 - 728 out 3
        if( x >= 780 && x <= 909){
            ledsOut3[x - 780] = leds[x];
        }

        //729 - 846 out 4,m mmn 
        if( x >= 910 && x <= 1026){
            ledsOut4[x - 910] = leds[x];
        }
        }


    //Give semaphore
    xSemaphoreGive(ready_to_display);
    deleteMarkedAnimations();
    //Reset the led strip
    fadeToBlackBy(leds, NUM_LEDS, 40);
    serialUpdates();
}

//ToDo: temp but works for now
void LEDHandler::bluetoothCheck() {
    int incoming;
    int number = 0;
    int layer;
    int writeType;
    animation* new_animation = handler_animation_list->start;
    if (!ESP_BT.available()) { return; }
    incoming = ESP_BT.read();
    Serial.print("Recieved"); Serial.println(incoming);
    if (incoming == 117) {//begin serial upload
        //Clear current handler
        clearHandler();

        while(1) {
            //clear loader
            handler_loader->clearLoader();
            //get id
            while (1) {
                if (ESP_BT.available() > 0) {
                    incoming = ESP_BT.read();
                    if (incoming == 'e') {
                        return;
                    }
                    if (incoming != '/') {
                        number = number * 10 + incoming - 48;
                        continue;
                    }
                    break;
                }
            }

            //Get proper animation from id
            for (int i = 0; i < number; i++) {
                new_animation = new_animation->next;
            }
            number = 0;
            //Load all variables
            while (1) {
                if (ESP_BT.available() > 0) {
                    incoming = ESP_BT.read();
                    if (incoming == 'o') {
                        break;
                    }
                    if (incoming == '/') {
                        handler_loader->append(number);
                        number = 0;
                        continue;
                    }
                    number = number * 10 + incoming - 48;
                }

            }			//get layer and write type
            while (1) {
                if (ESP_BT.available() > 0) {
                    incoming = ESP_BT.read();
                    if (incoming == '/') {
                        layer = number;
                        number = 0;
                        break;
                    }
                    number = number * 10 + incoming - 48;
                }
            }

            while (1) {
                if (ESP_BT.available() > 0) {
                    incoming = ESP_BT.read();
                    if (incoming == '/') {
                        writeType = number;
                        number = 0;
                        break;
                    }
                    number = number * 10 + incoming - 48;
                }
            }
            //add the animation
            addAnimation(new_animation, layer, handler_loader, writeType);
            new_animation = handler_animation_list->start;
        }
    }

}

/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Memory management~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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
    start = 0; //repurposing start variable to keep track of where variables should go
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

/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Adding Animations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int LEDHandler::addAnimation(animation* new_animation, int layer_index, Loader* loader, uint8_t writeType) {
    int x;
    int animation_index;
    int empty_in_row = 0;
    int full = 1;
    int start = 0;

    //Serial.println("ADDING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    //determine number of variables needed
    int num_variables = new_animation->determineNumVariables(loader, this);
    //Attempt to find a gap large enough to store all variables. If no gap is found defragment the variable memory, and check to see if it fits now
    for (x = 0; x < NUMBER_OF_VARIABLES; x++) {
        if (variable_in_use[x]) {
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
            return -1;
        }

    }
    else {
        start = x - num_variables + 1;
    }

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
        return -1;
    }

    animation_array[animation_index] = new_animation;
    variable_start_locations[animation_index] = start;
    start_time[animation_index] = current_time;
    number_of_variables[animation_index] = num_variables;
    animations_to_delete[animation_index] = false;
    write_type[animation_index] = writeType;
    layer[animation_index] = layer_index;

    //load variables
    for (int i = 0; i < num_variables; i++) {
        variable_in_use[start + i] = true;
        animation_variables[start + i] = loader->variables[i];
    }
    return animation_index;
}

/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Deleting Animations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void LEDHandler::deleteAnimation(int index){
    int num_variables = number_of_variables[index];
    int start_location = variable_start_locations[index];
    //Mark variables as not in use to "free" them
    for (int x = 0; x < num_variables; x++) {
        variable_in_use[start_location + x] = false;
    }
    //remove animation from the array
    animation_array[index] = NULL;
}

void LEDHandler::markForDeletion(int index) {
    animations_to_delete[index] = true;
}

void LEDHandler::deleteMarkedAnimations() {
    while(1){
        bool deleted = false;
        //Check each animation to see if it needs to be deleted. It can either be marked, or a dependency has been deleted
        for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
            if (animations_to_delete[x] == true) {
                deleteAnimation(x);
                animations_to_delete[x] = false;
                deleted = true;
            }
            else if (animation_array[dependencies[x]] == NULL && dependencies[x] != -1) {
                animations_to_delete[x] = false;
                deleteAnimation(x);
                deleted = true;
            }
        }
        //A dependency may of been deleted so run through again until no dependencies are deleted
        if (deleted) {
            //Serial.println("MORE");
            continue;
        }
        return;
    }

}

void LEDHandler::clearHandler() {
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        markForDeletion(x);
    }
    deleteMarkedAnimations();
}

/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Useful Resources for Animations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void LEDHandler::writeLed(int pos, uint8_t writeType, CRGB color) {
    if (pos >= NUM_LEDS) return;
    if (pos < 0) return;

    switch (writeType) {
        case 0://Zero for completely overwrite
            leds[pos] = color;
            break;
        case 1://one for additive write
            leds[pos] += color;
            break;
        case 2://brighter one wins
            if (leds[pos].getLuma() < color.getLuma()) {
                leds[pos] = color;
            }
            break;
        case 3: // average them
            leds[pos].r = (leds[pos].r + color.r) / 2;
            leds[pos].g = (leds[pos].g + color.g) / 2;
            leds[pos].b = (leds[pos].b + color.b) / 2;
            break;
    }
    //If greater brightness then overwrite, otherwise addt
}

/*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Playing Animations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void LEDHandler::playAnimations() {
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        play_order[x] = x;
    }
    mergeSort(play_order, 0, NUMBER_OF_ANIMATIONS - 1);
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        animation_index_number = play_order[x];
        if (animation_array[animation_index_number] != NULL) {
            //run the play function for this animation
            animation_array[animation_index_number]->play(variable_start_locations[animation_index_number], this);
        }
    }
}

void LEDHandler::mergeSort(int arr[], int l, int r) {
    if (l < r) {
        //Find mid
        int m = l + (r - l) / 2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        //Merge two halves
        merge(arr, l, m, r);
    }
}

void LEDHandler::merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (layer[L[i]] <= layer[R[j]]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Audio Data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Legacy Serial Code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void LEDHandler::serialAddAnimation() {
    int incoming = 0;
    int number = 0;
    int number_inputs;
    Serial.println("Enter Number Corresponding to Animation to Add");
    handler_animation_list->printAnimations();
    animation* animation_p = handler_animation_list->start;

    //Select the animation to add
    while (1) {
        if (Serial.available() > 0) {
            delay(2);
            incoming = Serial.read();
            if (incoming != 10) {
                number = number * 10 + incoming - 48;
                continue;
            }
            Serial.println("Recieved");
            Serial.println(number);
            break;
        }
    }
    for (int x = 0; x < number; x++) {
        animation_p = animation_p->next;
    }

    //Get inputs from user
    number_inputs = animation_p->num_inputs;
    handler_loader->clearLoader();

    number = 0;
    for (int y = 0; y < number_inputs; y++) {
        //gotta fill in the variables
        Serial.print("Enter Variable: ");
        Serial.println(y);
        while (1) {
            if (Serial.available() > 0) {
                delay(2);
                incoming = Serial.read();
                if (incoming != 10) {
                    number = number * 10 + incoming - 48;
                    continue;
                }
                Serial.println("Recieved: ");
                Serial.println(number);
                handler_loader->append(number);
                number = 0;
                break;
            }
        }
    }

    //Ask for write_type
    Serial.println("Enter a number for the write type");
    while (1) {
        if (Serial.available() > 0) {
            delay(2);
            incoming = Serial.read();
            if (incoming != 10) {
                number = number * 10 + incoming - 48;
                continue;
            }
            Serial.println("Recieved");
            Serial.println(number);
            break;
        }
    }
    uint8_t writeType = number;

    number = 0;
    //Ask for layer
    Serial.println("Enter a number for the layer");
    while (1) {
        if (Serial.available() > 0) {
            delay(2);
            incoming = Serial.read();
            if (incoming != 10) {
                number = number * 10 + incoming - 48;
                continue;
            }
            Serial.println("Recieved");
            Serial.println(number);
            break;
        }
    }

    //Add the Animation
    addAnimation(animation_p, number, handler_loader, writeType);
}

void LEDHandler::serialDeleteAnimation() {
    int number = 0;
    int incoming = 0;
    //Print loaded animations
    Serial.println("Enter a number corresponding to a animation to delete");
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        if (animation_array[x] != NULL) {
            Serial.print(x);
            Serial.print(": ");
            Serial.print(animation_array[x]->name);
            Serial.print(", Variable Start: ");
            Serial.print(variable_start_locations[x]);
            Serial.print(", Dependency: ");
            Serial.print(dependencies[x]);
            Serial.print(", Number Variables: ");
            Serial.print(number_of_variables[x]);
            Serial.print(", Layer: ");
            Serial.println(layer[x]);
            //Serial.print(", Layer Effect: ");
            //Serial.println(loaded_layers[layer_index_array[x]].effect->name);
        }

    }
    while (1) {
        if (Serial.available() > 0) {
            delay(2);
            incoming = Serial.read();
            if (incoming != 10) {
                number = number * 10 + incoming - 48;
                continue;
            }
            Serial.println("Recieved");
            Serial.println(number);
            break;
        }
    }
    markForDeletion(number);
}

void LEDHandler::serialUpdates() {
    int incoming;
    int number = 0;
    if (Serial.available() > 0) {
        while (Serial.available()) {
            incoming = Serial.read();
        }
        Serial.println("Enter 0 to add animation, 1 to delete animation, 2 to calibrate noise filters");
        while (1) {
            if (Serial.available() > 0) {
                delay(2);
                incoming = Serial.read();
                if (incoming != 10) {
                    number = number * 10 + incoming - 48;
                    continue;
                }
                Serial.println("Recieved");
                Serial.println(number);
                break;
            }
        }
        if (number == 0) {
            serialAddAnimation();
        }
        else if (number == 1) {
            serialDeleteAnimation();
        }
        else if (number == 2) {
            //handler_audio_data->calibrateNoise(leds);
        }
        else {
            printf("Invalid Selection");
        }
    }
    else {
        return;
    }
}

void LEDHandler::printInfo() {
    Serial.println("Printing Loaded Animations: ");
    for (int x = 0; x < NUMBER_OF_ANIMATIONS; x++) {
        if (animation_array[x] != NULL) {
            Serial.print(x);
            Serial.print(": ");
            Serial.print(animation_array[x]->name);
            Serial.print(", Variable Start: ");
            Serial.print(variable_start_locations[x]);
            Serial.print(", Dependency: ");
            Serial.print(dependencies[x]);
            Serial.print(", Number Variables: ");
            Serial.print(number_of_variables[x]);
            Serial.print(", Layer: ");
            Serial.println(layer[x]);
            //Serial.print(", Layer Effect: ");
            //Serial.println(loaded_layers[layer_index_array[x]].effect->name);
        }

    }
    Serial.println("");
    Serial.println("Printing Loaded Variables:");
    for (int x = 0; x < NUMBER_OF_VARIABLES; x++) {
        if (variable_in_use[x] == true) {
            Serial.print(x);
            Serial.print(": ");
            Serial.println(animation_variables[x]);
        }
    }
}
