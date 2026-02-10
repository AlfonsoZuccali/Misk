#include "inputManager.h"

InputManager :: InputManager(){
    _btnAction = nullptr;
    _btnMode = nullptr;
    _btnReset = nullptr;
    _btnSkip = nullptr;
}

void InputManager :: begin(QueueHandle_t queue){
    _commandQueue = queue;

    _btnMode   = new Button(PIN_MODE,   50, 500);
    _btnAction = new Button(PIN_ACTION, 50, 500);
    _btnSkip   = new Button(PIN_SKIP,   50, 500);
    _btnReset  = new Button(PIN_RESET,  50, 500);

    xTaskCreate(
        taskLoop,
        "InputTask",
        2048,
        this,
        1,
        NULL
    );

    Serial.println("[InputManager] Iniciado con Polling.");
}

void InputManager :: taskLoop(void* pvParameters){
    InputManager* self = (InputManager*)pvParameters;

    for(;;){
        self->_btnAction->update();
        self->_btnMode->update();
        self->_btnReset->update();
        self->_btnSkip->update();

        self->checkAndSend(self->_btnMode,   InputEvent::BTN_MODE_SHORT,   InputEvent::BTN_MODE_LONG);
        self->checkAndSend(self->_btnAction, InputEvent::BTN_ACTION_SHORT, InputEvent::BTN_ACTION_LONG);
        self->checkAndSend(self->_btnSkip,   InputEvent::BTN_SKIP_SHORT,   InputEvent::BTN_SKIP_LONG);
        self->checkAndSend(self->_btnReset,  InputEvent::BTN_RESET_SHORT,  InputEvent::BTN_RESET_LONG);

        // 3. Dormir un poco (Polling Rate)
        // 20ms es imperceptible para el humano pero libera CPU
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void InputManager :: checkAndSend(Button* button, InputEvent shortEvent, InputEvent longEvent){
    SystemCommand cmd;
    bool eventDetected = false;

    if(button->isShortPressed()){
        cmd.type = shortEvent;
        eventDetected = true;
        Serial.println("[Input] Short Press by pin");
        Serial.println(button->getPin());
    }else if(button->isLongPressed()){
        cmd.type = longEvent;
        eventDetected = true;
        Serial.println("[Input] Long Press by pin");
        Serial.println(button->getPin());
    }

    if(eventDetected){
        xQueueSend(_commandQueue, &cmd, 0);
    }
}