#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button{
    private:
        int _pin;
        unsigned long _debounceMs;
        unsigned long _longPressMs;

        int _lastSteadyState;
        int _lastFlickerableState;
        unsigned long _lastDebounceTime;
        unsigned long _startPressTime;

        bool _shortPressFlag;
        bool _longPressFlag;

    public:
        Button(int pin, unsigned long debounceMs = 50, unsigned long longPressMs = 800);
        
        void update();

        bool isShortPressed();
        bool isLongPressed();
        int getPin();
};


#endif