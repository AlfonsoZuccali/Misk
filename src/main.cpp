#include <Arduino.h>
#include "wifiManager.h"
#include "secrets.h"

WiFiManager wifiManager(WIFI_SSID,WIFI_PASSWORD);
//we define the event group that wil be used by the system
EventGroupHandle_t systemEventGroup;

void setup(){
  Serial.begin(225200);

  //we create the event group
  systemEventGroup = xEventGroupCreate();

  //we initiate the wifi and delegate the task creation to the object
  wifiManager.startConnectionTask(systemEventGroup);
}

void loop(){
  vTaskDelete(NULL);
}