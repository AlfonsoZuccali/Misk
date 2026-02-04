#include <WiFiManager.h>
#include <systemEvents.h>

WiFiManager :: WiFiManager(const char* ssid, const char* password){
    _ssid = ssid;
    _password = password;
}

void WiFiManager :: startConnectionTask(EventGroupHandle_t eventGroup) {
    _eventGroup = eventGroup; // Guardamos la referencia para usarla luego

    // we give "this" as a parameter for the task
    xTaskCreate(
        taskLoop,           // static function
        "WiFiTask",         // task name
        4096,               // Stack
        this,               // IMPORTANT: we pass down this object to the task
        1,                  // low priority
        NULL
    );
}

bool WiFiManager::isConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

String WiFiManager::getIP() {
    return WiFi.localIP().toString();
}

// --- task that freeRTOS runs ---
void WiFiManager :: taskLoop(void* pvParameters) {

    // 1. we recover the object pointer from void
    WiFiManager* self = (WiFiManager*)pvParameters;

    // we now have to use the "self" pointer that points to the object.
    // this is because we are in a static function 
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(self->_ssid, self->_password);

    Serial.println("[WiFi Task] Iniciada.");

    for (;;) {
        if (self->isConnected()) {
            // we use the EventGroup defined at the class
            xEventGroupSetBits(self->_eventGroup, WIFI_CONNECTED_BIT);
        } else {
            xEventGroupClearBits(self->_eventGroup, WIFI_CONNECTED_BIT);
            
            //if we cant connect we clear the bits from the EventGroup
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}