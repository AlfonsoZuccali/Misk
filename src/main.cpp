#include <Arduino.h>
#include "WiFiManager.h"
#include "secrets.h"
#include "timeManager.h"
#include "DisplayManager.h"
#include "inputManager.h"
#include "pomodoroManager.h"

WiFiManager wifiManager(WIFI_SSID,WIFI_PASSWORD);
TimeManager timeManager(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
DisplayManager displayManager;
InputManager inputManager;
PomodoroManager pomodoroManager;

//we define the event group that wil be used by the system
EventGroupHandle_t systemEventGroup;
QueueHandle_t commandQueue;

void setup(){
  Serial.begin(115200);

  //we create the event group
  systemEventGroup = xEventGroupCreate();
  commandQueue = xQueueCreate(10, sizeof(SystemCommand));

  //we initiate the wifi and delegate the task creation to the object
  wifiManager.startConnectionTask(systemEventGroup);
  timeManager.startTimeTask(systemEventGroup);

  inputManager.begin(commandQueue);     
  pomodoroManager.begin(commandQueue, 25, 5, 15, 4, 12);

  displayManager.startTask();
}

void loop(){
  vTaskDelete(NULL);
}