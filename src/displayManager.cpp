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
    
    // Prueba de color visual
    _tft.fillScreen(TFT_RED);
    vTaskDelay(pdMS_TO_TICKS(500));
    _tft.fillScreen(TFT_GREEN);
    vTaskDelay(pdMS_TO_TICKS(500));
    _tft.fillScreen(TFT_BLUE);
    vTaskDelay(pdMS_TO_TICKS(500));
    
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.drawString("MiMo ST7735", 10, 10, 2);
    Serial.println("[Display] Test finalizado.");
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
    vTaskDelay(pdMS_TO_TICKS(1000)); //Initialization text shown for a second
    
    for(;;){
        
        //wait for the event group bits to enter the correct state
        EventBits_t bits = xEventGroupGetBits(systemEventGroup);
        bool wifiReady = (bits & WIFI_CONNECTED_BIT);
        bool timeReady = (bits & TIME_SYNCED_BIT);
        
        Serial.print("BITS: ");
        Serial.println(bits, BIN);

        Serial.print("TIME MASK: ");
        Serial.println(TIME_SYNCED_BIT, BIN);


        
        //get the current time from the time manager
        String currentTime = "--:--";
        if(timeReady){
            currentTime = timeManager.getFormattedTime();
        }

        //draw
        self->drawClock(currentTime);
        self->drawWifiStatusIcon(wifiReady, timeReady);

        //refresh rate
        vTaskDelay(pdMS_TO_TICKS(200)); // every 200ms
    }
}

void DisplayManager :: drawClock(String timeString){
    if(timeString != _lastTime){
        _tft.setTextDatum(MC_DATUM); //center text
        _tft.setTextColor(TFT_CYAN, TFT_BLACK);
        _tft.drawString(timeString, 80, 64, 4);

        _lastTime = timeString;
    }
}

void DisplayManager :: drawWifiStatusIcon(bool wifiConnected, bool timeSynced){
    if (wifiConnected != _lastWifiState){
        if(wifiConnected){
            _tft.fillCircle(150,10,3, TFT_GREEN);
        }else{
            _tft.fillCircle(150, 10, 3, TFT_RED);
        }
        _lastWifiState = wifiConnected;
    }
}