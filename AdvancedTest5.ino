#include <AdvancedLED5.h>
#include <Animations.h>
#include <AudioSampling.h>
#include <LEDHandler.h>
#include <Loader.h>
#include <Utilities.h>



LEDHandler mainHandler;
void handlerLoop(void * param){   
    while(1){
        mainHandler.run();
    }
}

void displayLoop(void * param){
    while(1){
        //Wait until the CRGB array has been loaded
        //Serial.println("3");
        xSemaphoreTake(ready_to_display, portMAX_DELAY);
        //Serial.println("4");
        //Display the new leds
        FastLED.show();
        xSemaphoreGive(ready_to_load);
        //Delay this task until enough time has passed to keep a consistent frame rate
        vTaskDelay((1000/FRAMES_PER_SECOND)/portTICK_RATE_MS);
    }
}

void setup(){
    // put your setup code here, to run once:
    delay(1000);
    Serial.begin(115200);
    mainHandler.initializeHandler();
    disableCore0WDT();
    xTaskCreatePinnedToCore(FFTLoop, "FFT Task", 10000, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(displayLoop, "display Task", 10000, NULL, configMAX_PRIORITIES - 1 , NULL, 1);
    xTaskCreatePinnedToCore(handlerLoop, "write Task", 10000, NULL, 5, NULL, 1);
}

void loop(){
    vTaskDelay(10000000/portTICK_RATE_MS);
}
