#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "uiAssets.h"

enum class DisplayState{
    BOOT_LOGO,
    CLOCK_SCREEN,
    POMODORO_WORK,
    POMODORO_BREAK,
    SPOTIFY_PLAYER
};

class DisplayManager{
    private:
        TFT_eSPI _tft;
        DisplayState  _currentDisplayState;
        DisplayState  _lastState;


        String _lastTime;
        bool _lastWifiState;

        static void taskLoop(void* pvParameters);
        void renderBootScreen();
        void renderClockScreen(String time, bool wifi);
        void renderPomodoroScreen(String time, float progress, bool isWork);
    
    public:
        DisplayManager();
        void begin();

        void startTask(); 
        void setScreen(DisplayState newState);       
};

#endif