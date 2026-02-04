#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

class TimeManager{

    private:
        const char* _ntpServer;
        long _gmtOffset_sec;
        int _daylightOffset_sec;
        EventGroupHandle_t _eventGroup;

        static void taskLoop(void* pvParameters);

    public:
        TimeManager(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec);

        void startTask(EventGroupHandle_t eventGroup);

        String getFormattedTime();

        String getFormattedDate();
};

#endif