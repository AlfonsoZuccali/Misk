#include "pomodoroManager.h"
#include "systemEvents.h"

PomodoroManager :: PomodoroManager(){
    
    _currentState = PomodoroState :: IDLE;

    _roundsBeforeLong = 0;
    _roundsCompleted = 0;
    _roundsObjective = 0;

    _focusDurationMs = 25 * 60 * 1000;
    _shortBreakDurationMs = 5 * 60 * 1000;
    _longBreakDurationMs = 15 * 60 * 1000;

    _timer.reset();
}

void PomodoroManager :: begin(QueueHandle_t queue,uint8_t focusDuration, uint8_t shortBreakDuration, uint8_t longBreakDuration ,uint8_t roundsForLong, uint8_t roundsObjective){
    
    _commandQueue = queue;
    
    _roundsBeforeLong = roundsForLong;
    _roundsObjective = roundsObjective;

    _longBreakDurationMs = longBreakDuration *60 *1000;
    _shortBreakDurationMs = shortBreakDuration *60 *1000;
    _focusDurationMs = focusDuration *60 *1000;

    xTaskCreate(
        taskLoop,
        "PomodoroTask",
        4096,
        this,
        1,
        NULL
    );
}

void PomodoroManager :: taskLoop(void* pvParameters){
    PomodoroManager* self = (PomodoroManager*)pvParameters;
    SystemCommand cmd;

    for(;;){
       // 1. Process button commands (max wait 100ms)
        if (xQueueReceive(self->_commandQueue, &cmd, pdMS_TO_TICKS(100)) == pdTRUE) {
            self->processCommand(cmd.type);
        }

        // 2. verify timer
        if (self->_currentState != PomodoroState::IDLE) {
            
            if (self->_timer.isFinished()) {
                self->transitionToNextState();
            }
        } 
    }
}

void PomodoroManager :: processCommand(InputEvent cmd){
    switch (cmd) {
        case InputEvent::BTN_ACTION_SHORT:
            actionButton();
            break;
        case InputEvent::BTN_SKIP_SHORT:
            skipButton();
            break;
        case InputEvent::BTN_RESET_LONG:
            resetSession();
            break;
        case InputEvent :: BTN_RESET_SHORT:
            resetRound();
        default:
            break;
    }
}

void PomodoroManager :: actionButton(){
    if(_currentState == PomodoroState :: IDLE){
        startSession();
    }else{
        if(_timer.isRunning()){
            _timer.pause();
            Serial.println("[POMO] paused");
        }else{
            _timer.resume();
            Serial.println("[POMO] resumed");
        }
    }
}

void PomodoroManager :: skipButton(){
    if(_currentState != PomodoroState :: IDLE){
        transitionToNextState();
        Serial.println("[POMO] skipped");
    }
}

void PomodoroManager :: resetSession(){
    stopSession();
    Serial.println("[POMO] reseted");
}

void PomodoroManager :: resetRound(){
    if(getState() != PomodoroState :: IDLE ){
        _timer.reset();
        switch(getState()){
            case PomodoroState :: FOCUS:
                _timer.start(_focusDurationMs);
                break;
            case PomodoroState :: LONG_BREAK:
                _timer.start(_longBreakDurationMs);
                break;
            case PomodoroState :: SHORT_BREAK:
                _timer.start(_shortBreakDurationMs);
                break;
        }
    }   
    _timer.pause();
}

void PomodoroManager :: startSession(){
    _roundsCompleted = 0;
    _currentState = PomodoroState ::FOCUS;

    //set timer to zero
    _timer.reset();

    //we start it with the focus time
    _timer.start(_focusDurationMs);
    //we inmediatly pause it
    _timer.pause();
    Serial.println("[POMO] Sesión Iniciada: FOCUS");
}

void PomodoroManager :: stopSession(){
    _timer.reset();
    _currentState = PomodoroState :: IDLE;
    _roundsCompleted = 0;
    Serial.println("[POMO] Sesión Cancelada -> IDLE");
}

void PomodoroManager :: transitionToNextState(){
    _timer.reset();

    if (_currentState == PomodoroState::FOCUS) {
        _roundsCompleted++;
        Serial.printf("[POMO] Ronda %d completada.\n", _roundsCompleted);

        // check for long breaks
        // if we completed at least one round and _roundsBeforeLong is a divider of roundsCompleted, it takes the long break
        if (_roundsCompleted > 0 && (_roundsCompleted % _roundsBeforeLong == 0)) {
            _currentState = PomodoroState::LONG_BREAK;
            _timer.start(_longBreakDurationMs);
            Serial.println("[POMO] Estado: LONG BREAK");
        } else {
            _currentState = PomodoroState::SHORT_BREAK;
            _timer.start(_shortBreakDurationMs);
            Serial.println("[POMO] Estado: SHORT BREAK");
        }
    }else if (_currentState == PomodoroState::SHORT_BREAK || _currentState == PomodoroState::LONG_BREAK) {
        
        // verify if we completed the session
        if (_roundsCompleted >= _roundsObjective) {
            stopSession();
            return;
        }
        _currentState = PomodoroState::FOCUS;
        _timer.start(_focusDurationMs);
        Serial.println("[POMO] Estado: FOCUS");
    }

    //every state begins paused, that way we wait for users input to start the timer
    _timer.pause();
}

//----------getters----------

PomodoroState PomodoroManager::getState() {
    return _currentState;
}

bool PomodoroManager::isPaused() {
    
    if (_currentState == PomodoroState::IDLE) return false;
    return !_timer.isRunning();
}

int PomodoroManager::getRounds() {
    return _roundsCompleted;
}

String PomodoroManager::getFormattedTime() {
    if (_currentState == PomodoroState::IDLE) {
        return "--:--";
    }

    unsigned long remaining = _timer.getRemainingTime();
    unsigned long seconds = remaining / 1000;
    unsigned long m = seconds / 60;
    unsigned long s = seconds % 60;

    char buf[6];
    sprintf(buf, "%02lu:%02lu", m, s);
    return String(buf);
}


float PomodoroManager::getProgress() {
    if (_currentState == PomodoroState::IDLE) return 0.0;
    
    unsigned long currentDuration = 0;
    switch (_currentState) {
        case PomodoroState::FOCUS: currentDuration = _focusDurationMs; break;
        case PomodoroState::SHORT_BREAK: currentDuration = _shortBreakDurationMs; break;
        case PomodoroState::LONG_BREAK: currentDuration = _longBreakDurationMs; break;
        default: return 1.0;
    }
    
    if (currentDuration == 0) return 0.0;
    
    unsigned long remaining = _timer.getRemainingTime();
    return 1.0 - ((float)remaining / (float)currentDuration);
}