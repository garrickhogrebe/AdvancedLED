#include "Loader.h"

Loader main_loader;

void Loader::clearLoader() {
    num_loaded = 0; //Actually clearing the array isnt neccesary so long as careful with this variable
}

void Loader::append(int new_int) {
    variables[num_loaded++] = new_int;
}