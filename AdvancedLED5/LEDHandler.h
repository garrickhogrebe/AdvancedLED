#ifndef AL_LEDHandler_H
#define AL_LEDHandler_H
#include "AdvancedLED5.h"
#include "Animations.h"
#include "Loader.h"
#include "AudioSampling.h"
#include "AudioData.h"

//The LEDHandler class is the main meat of this program
//It is responsible for memory allocation of all the animations; running adding and deleting animations;
//Beat detection from the audio data from the other core; and other things
class LEDHandler {
public:

    //ToDo: Better comments lol

    //Bluetooth
    BluetoothSerial ESP_BT;

    //Array of LEDS for the animations to write too.
    CRGB leds[NUM_LEDS];

    /*
    CRGB ledsOut1[NUM_LEDS];
    CRGB ledsOut2[NUM_LEDS];
    CRGB ledsOut3[NUM_LEDS];
    CRGB ledsOut4[NUM_LEDS];
    */

    CRGB ledsOut1[402];
    CRGB ledsOut2[378];
    CRGB ledsOut3[131];
    CRGB ledsOut4[117];



    //List of Possible Animations to choose from
    class animation_list *handler_animation_list;

    //initializes the LEDHandler
    void initializeHandler();

    //runs a full cycle of what needs to be done by the handler each frame
    void run();

    //checks for bluetooth message and does proper updates
    void bluetoothCheck();

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Memory management~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Frequent Dynamic memory allocation and freeing on microcontrollers is not advised, the memory can easily become fragmented and problems
    * will arise. To avoid any dynamic allocation, the LED handler allocates a large amount of memory at compile time.
    * When a new animation needs to be created, the handler searches the pre allocated memory to try and find where it
    * can store the memory needed to run that animation. The following methods and variables pertain to this process.
    * There is an array of animation pointers which tell you what animations are currently loaded, an array of integers,
    * where the variables for each animation are stored, and an array of memory locations telling each animation where
    * their variables can be found
    * ToDo: Allow for animations to request and free memory while they are running - will allow for more complex animations
    * ToDo: Have a one time dynamic allocation at runtime to get more MEMORIES
    */

    //Array containing all currently loaded animations
    class animation *animation_array[NUMBER_OF_ANIMATIONS];

    //Array showing each animation where the start of it's variables are stored in pre allocated animation variable array
    int variable_start_locations[NUMBER_OF_ANIMATIONS];

    //Array containing how many variables each loaded animation has - useful when deleting the animation
    int number_of_variables[NUMBER_OF_ANIMATIONS];

    //Array where the variables for each loaded animation are stored
    int animation_variables[NUMBER_OF_VARIABLES];

    //Array specifying if the location in the animation_variables array is being used or not - Useful when allocating memory for animations
    bool variable_in_use[NUMBER_OF_VARIABLES];

    //Defragments the variable array, returns the first available memory location
    int cleanVariableArray();

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Adding Animations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* When an animation is "added", a pointer to it is placed on the animation array.
     * Every frame, the handler will loop through the entire animation loop to play these animations
     * When an animations needs to be added, whether by another animation or through user input, this function is called
     * ~~Parameters~~
     * new_animations: Pointer to the animation object to be added to the array
     * layer_index: Decides the order in which animations are ran by the handler. Lower layers are ran first
     * loader: Special class used load an animations variables. See "Loader.h"
     * writeType: Animations can be set to write to the strip in different way. Ex: blend with other animations, or overwrite other animations
     * ToDo: Add dependencies to addAnimation
     */
    int addAnimation(animation *new_animation, int layer_index, Loader *loader, uint8_t writeType = 1);

    //The loader for this handler ToDo: maybe just take this out of addAnimation parameter
    class Loader *handler_loader;

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Deleting Animations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* To avoid various issues, animations are not deleted in the middle of a cycle through the animation array.
     * Instead, they are marked for deletion and then all deleted after all animations have been played.
     * Some animations are dependent on other animations.
     * As such, a dependency can be set for each animation.
     * This ensures that if an animation one animation is dependent on gets deleted, the dependent one is also deleted
     */

    //Array indicating that an animation has been marked for deletion
    bool animations_to_delete[NUMBER_OF_ANIMATIONS];

    //Array Containing dependencies for deleting animations. ToDo: A value of -1 indicates is independent
    int dependencies[NUMBER_OF_ANIMATIONS];

    //Deletes a single animation at the given index by setting the pointer value at that index in the animation array to NULL.
    //Also frees the memory the animation was using in the animation variables array
    //Not intended to be called by other animations - use markForDeletion
    void deleteAnimation(int index);

    //Marks an animation for deletion. This animation will be deleted at the end of this handler cycle
    void markForDeletion(int index);

    //Cycles through the animations_to_delete array and deletes all animations as well as their dependencies
    void deleteMarkedAnimations();

    //Completely deletes all animations from the animation array and frees all of the animation variables
    void clearHandler();

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Useful Resources for Animations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* When it is each animation's turn to be ran in the handler, they are able to access the following useful information. The global animation_index_number
     * is always kept updated so that when the animation runs it can use it to access this data
     */

    //Variable specifying the index of whatever animation is currently being played can be accessed by each animation when running to find where it is located
    //in the animation array ToDo: int is overkill
    int animation_index_number;

    //Variable which holds the current time. Determined at the start of each run cycle
    unsigned long int current_time;

    //Array containing time values for each animation. When an animation is loaded this is initialized with the time loaded, but I frequently
    //make animations update this variable to keep track of new time information. Should probably rename
    unsigned long int start_time[NUMBER_OF_ANIMATIONS];

    //Array containing a code for how to write each animation - blend with previous animations, overwrite them, etc
    uint8_t write_type[NUMBER_OF_ANIMATIONS];

    //Array which states which layer each animation is on. This is used to determine the order each animation is played. Lower values get played first ToDo: Make float
    int layer[NUMBER_OF_ANIMATIONS];

    //Write an led ToDo: currently sucks lol, investigate better methods
    void writeLed(int pos, uint8_t writeType, CRGB color);

    /*!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Playing Animations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* To "play" each animation, animations are first sorted by their layer ToDo: Only sort when new animation is added, not every cycle
     * They are then ran through one by one and the function pointer for the each animation is called on with the location of where to find each
     * animations variables as a parameter
     */

    //Calls the merge sort functions to determine order, then goes through each animations and calls the function pointer
    void playAnimations();

    //Sorted array which tells the handler which order to run the animations ToDo: int is overkill
    int play_order[NUMBER_OF_ANIMATIONS];

    //Sorts the animations by layers using merge sort, results stored in play_order array
    void mergeSort(int arr[], int l, int r);

    //Merge function for merge sort
    void merge(int arr[], int l, int m, int r);

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Audio Data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* ToDo: Comments for this section */

    //Handlers audio data
    class audioData *hAD;

    /*! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Legacy Serial Code ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Some once useful code for running the engine on the serial monitor UI. Not really useful now that app is used but sometimes helpful for debugging
    */
    //Load a new animation through the serial console
    void serialAddAnimation();

    //Delete an animation through the serial console
    void serialDeleteAnimation();

    //Get user instructions from the serial monitor
    void serialUpdates();


    //Prints currently loaded animations and their layer. Prints variables. Useful for debugging
    void printInfo();
};

#endif

