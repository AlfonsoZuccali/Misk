#ifndef POMODORO_MANAGER_H
#define POMODORO_MANAGER_H

#include "inputManager.h"
#include <Arduino.h>
#include "timer.h"

enum class PomodoroState{
    IDLE,
    FOCUS,
    SHORT_BREAK,
    LONG_BREAK
};

class PomodoroManager{
    private:
        //they use ms for configuring timer
        uint32_t _longBreakDurationMs;
        uint32_t _shortBreakDurationMs;
        uint32_t _focusDurationMs;

        //actual state
        Timer _timer;
        PomodoroState _currentState;
        uint8_t _roundsCompleted;

        //round check
        uint8_t _roundsObjective;       
        uint8_t _roundsBeforeLong;

        //internal logic
        void transitionToNextState();
        void startSession();
        void stopSession();

        //freeRTOS
        QueueHandle_t _commandQueue;
        static void taskLoop(void* pvParameters);
        void processCommand(InputEvent cmd);
    
    public:
        PomodoroManager();

        //begin takes parameters in minutes
        void begin(QueueHandle_t queue, 
               uint8_t focusMin = 25, 
               uint8_t shortBreakMin = 5, 
               uint8_t longBreakMin = 15,
               uint8_t roundsLong = 4, 
               uint8_t roundsObj = 12);

        //BUTTONS
        //start, pause, resume
        void actionButton();
        //skip round
        void skipButton();
        //reset round(shortPress) / reset Session(longPress)
        void resetSession();
        void resetRound();

        //getters for the UI
        PomodoroState getState();
        String getFormattedTime(); // MM:SS
        bool isPaused();           // for showing pause icon
        int getRounds();           // completed rounds
        float getProgress();       // returns a decimal from 0.0 to 0.1 for potential progress bar
};

#endif