#include <Arduino.h>
#include "WiFiManager.h"
#include "secrets.h"
#include "timeManager.h"

WiFiManager wifiManager(WIFI_SSID,WIFI_PASSWORD);
TimeManager timeManager(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
//we define the event group that wil be used by the system
EventGroupHandle_t systemEventGroup;

void setup(){
  Serial.begin(225200);

  //we create the event group
  systemEventGroup = xEventGroupCreate();

  //we initiate the wifi and delegate the task creation to the object
  wifiManager.startConnectionTask(systemEventGroup);
  timeManager.startTimeTask(systemEventGroup);
}

void loop(){
  vTaskDelete(NULL);
}