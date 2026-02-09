#include "button.h"

Button :: Button(int pin, unsigned long debounceMs, unsigned long longPressMs){
    _pin = pin;
    _debounceMs = debounceMs;
    _longPressMs = longPressMs;

    pinMode(_pin, INPUT_PULLUP);

    _lastSteadyState = HIGH;
    _lastFlickerableState = HIGH;
    _lastDebounceTime = 0;
    _startPressTime = 0;

    _shortPressFlag = false;
    _longPressFlag = false;
}

void Button :: update(){
    int currentState = digitalRead(_pin);

    if(currentState != _lastFlickerableState){
        _lastDebounceTime = millis();
        _lastFlickerableState = currentState;
    }

    if((millis() - _lastDebounceTime) > _debounceMs){
        if(currentState != _lastSteadyState){

            //is pressing
            if (_lastSteadyState == HIGH && currentState == LOW) {
                _startPressTime = millis(); 
            }else if (_lastSteadyState == LOW && currentState == HIGH) {
                
                unsigned long duration = millis() - _startPressTime;

                if (duration >= _longPressMs) {
                    _longPressFlag = true;
                } else {
                    _shortPressFlag = true;
                }
            }
            
            _lastSteadyState = currentState;
        }
    }
} 

bool Button :: isLongPressed(){
    if(_longPressFlag){
        _longPressFlag = false;
        return true;
    }else{
        return false;
    }
}

bool Button :: isShortPressed(){
    if(_shortPressFlag){
        _shortPressFlag = false;
        return true;
    }else{
        return false;
    }
}

int Button :: getPin(){
    return _pin;
};