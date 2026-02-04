#include <timeManager.h>
#include <systemEvents.h>
#include <time.h>

TimeManager :: TimeManager(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec){
    _ntpServer = ntpServer;
    _gmtOffset_sec = gmtOffset_sec;
    _daylightOffset_sec = daylightOffset_sec;
}

void TimeManager :: startTimeTask(EventGroupHandle_t eventGroup){
    _eventGroup = eventGroup;

    xTaskCreate(
        taskLoop,
        "TimeTask",
        3072,
        this,
        1,
        NULL
    );
}

void TimeManager :: taskLoop(void* pvParameters){
    TimeManager* self = (TimeManager*)pvParameters;

    //this functions makes the object wait for the corresponding bit in the event group to change 
    Serial.print("Waiting for WiFi conection for time sync... ");
    xEventGroupWaitBits(
        self->_eventGroup,  //event group
        WIFI_CONNECTED_BIT, //first bit(wifi state bit)
        pdFALSE,            //dont clear any bits within the event group 
        pdTRUE,             //wait for all bits to react
        portMAX_DELAY       //take all time needed
    );

    Serial.print("WiFi detected, intializing time...");

    //we set the ESP-32 internal clock to the time we got from the NTP
    configTime(self->_gmtOffset_sec, self->_daylightOffset_sec, self->_ntpServer);
    
    //THIS TWO LINES ARE HARDCODED FOR ARGENTINA, PLEASE DO YOUR OWN
    //RESEARCH FOR COMPLETING THEM FOR YOUR TIMEZONE
    setenv("TZ","<-03>3",1);
    tzset();

    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        Serial.println("Syncing NTP...");
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }

    Serial.print("Time synced!");

    for (;;) {
        // Solo para debug por ahora: imprimimos la hora cada 5 seg
        Serial.println(self->getFormattedTime());
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

String TimeManager::getFormattedTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        return "--:--";
    }
    char timeStringBuff[9]; // HH:MM:SS + null terminator
    strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S", &timeinfo);
    return String(timeStringBuff);
}