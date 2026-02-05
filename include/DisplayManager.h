#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>


class DisplayManager{
    private:
        TFT_eSPI _tft;

        String _lastTime;
        bool _lastWifiState;

        static void taskLoop(void* pvParameters);
    
    public:
        DisplayManager();
        void begin();

        void startTask();

        void drawClock(String timeString);
        void drawWifiStatusIcon(bool wifiConnected, bool tymeSynced);
};

#endif