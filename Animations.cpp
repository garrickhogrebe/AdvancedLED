#include "Animations.h"

animation::animation() {
	
}

int animation::determineNumVariables(Loader* loader) {
	if (standard_load) {
		return loader->num_loaded;
	}
	else {
		altLoad(loader);
		return loader->num_loaded;
	}
}

void animation_list::append(class animation* new_animation) {

}