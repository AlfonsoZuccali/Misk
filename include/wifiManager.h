#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
private:
    const char* _ssid;
    const char* _password;
    EventGroupHandle_t _eventGroup; // Reference to the event group

    //static method that freeRTOS will run
    static void taskLoop(void* pvParameters);

public:
    WiFiManager(const char* ssid, const char* password);
    
    //saves reference to the event group
    void startConnectionTask(EventGroupHandle_t eventGroup);
    
    bool isConnected();
    

    String getIP();
};

#endif