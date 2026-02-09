#include "DisplayManager.h"
#include "SystemEvents.h"
#include "TimeManager.h" 
#include "WiFiManager.h"


extern TimeManager timeManager;
extern EventGroupHandle_t systemEventGroup;

DisplayManager :: DisplayManager(): _tft(TFT_eSPI()){
    _lastTime = "";
    _lastWifiState = false;
}

void DisplayManager :: begin(){
    Serial.println("[Display] Inicializando ST7735...");
    
    pinMode(4, OUTPUT); 
    digitalWrite(4, HIGH);
    vTaskDelay(pdMS_TO_TICKS(50));
    digitalWrite(4, LOW);
    vTaskDelay(pdMS_TO_TICKS(200)); // 200ms apagado (antes 100)
    digitalWrite(4, HIGH);
    vTaskDelay(pdMS_TO_TICKS(200)); // 200ms para despertar (antes 100)

    _tft.init();
    _tft.setRotation(1); // 0=Portrait, 1=Landscape
    _tft.fillScreen(TFT_BLACK);
}

void DisplayManager :: startTask(){
    xTaskCreate(
        taskLoop,
        "UITask",
        4096,
        this,
        1,
        NULL
    );
}

void DisplayManager :: taskLoop(void* pvParameters){
    DisplayManager* self = (DisplayManager*)pvParameters;

    self->begin();
    self->_currentDisplayState = DisplayState :: BOOT_LOGO;
    vTaskDelay(pdMS_TO_TICKS(1000)); //Initialization text shown for a second


    for(;;){
        
        //wait for the event group bits to enter the correct state
        EventBits_t bits = xEventGroupGetBits(systemEventGroup);
        bool wifiReady = (bits & WIFI_CONNECTED_BIT);
        bool timeReady = (bits & TIME_SYNCED_BIT);

        //get the current time from the time manager
        String currentTime = "--:--";
        if(timeReady){
            currentTime = timeManager.getFormattedTime();
        }

        switch (self->_currentDisplayState)
        {
            case DisplayState :: BOOT_LOGO:
                self->renderBootScreen();
                vTaskDelay(pdMS_TO_TICKS(3000));
                self->_lastState = self->_currentDisplayState;
                self->_currentDisplayState = DisplayState :: CLOCK_SCREEN;
                break;

            case DisplayState :: CLOCK_SCREEN:
                self->renderClockScreen(currentTime, wifiReady);
                break;
        }

        //refresh rate
        vTaskDelay(pdMS_TO_TICKS(200)); // every 200ms
    }
}

void DisplayManager :: renderBootScreen(){

    _tft.fillScreen(0x0);
    // background
    _tft.fillRect(0, 0, 160, 128, MISK_RED);
    // Layer 1
    _tft.setTextColor(0xFFFF);
    _tft.setTextSize(3);
    _tft.setFreeFont();
    _tft.drawString("Misk", 46, 54);
    // Layer 2
    _tft.setTextSize(1);
    _tft.drawString("booting", 60, 115);
}

void DisplayManager :: renderClockScreen(String time, bool wifiReady){
    
   if(_lastTime != time){
        _tft.fillScreen(0x0);
        // Layer 1
        _tft.setTextColor(0xFFFF);
        _tft.setTextSize(5);
        _tft.setFreeFont();
        _tft.drawString(time, 8, 47);
        _lastTime = time;
   }
    
    // wifi_full
    if(wifiReady){
        _tft.drawBitmap(141, 0, image_wifi_full_bits, 19, 16, MISK_GREEN);
    }else{
        // wifi_not_connected
        _tft.drawBitmap(141, 0, image_wifi_not_connected_bits, 19, 16, MISK_RED);
    }
}