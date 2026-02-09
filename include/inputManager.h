#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
#include <Arduino.h>
#include "systemEvents.h"
#include "button.h"

class InputManager{
    private:
       QueueHandle_t _commandQueue;


       const int PIN_MODE = 27;
       const int PIN_ACTION = 12;
       const int PIN_RESET = 13;
       const int PIN_SKIP = 14;

       Button* _btnMode;
       Button* _btnAction;
       Button* _btnReset;
       Button* _btnSkip;

       static void taskLoop(void* pvParameters);

       void checkAndSend(Button* button, InputEvent shortEvent, InputEvent longEvent);
    
    public:
        InputManager();

        void begin(QueueHandle_t queue);
};

#endif