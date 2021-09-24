#include "AdvancedLED5.h"

SemaphoreHandle_t full_buffers = xSemaphoreCreateCounting(1, 0);
SemaphoreHandle_t empty_buffers = xSemaphoreCreateCounting(1, 1);
SemaphoreHandle_t ready_to_display = xSemaphoreCreateCounting(1, 0);
SemaphoreHandle_t ready_to_load = xSemaphoreCreateCounting(1, 1);


