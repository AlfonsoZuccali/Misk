# Misk: Advanced Desktop Productivity OS

![Status](https://img.shields.io/badge/Status-Alpha-yellow)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino%20%7C%20FreeRTOS-green)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

**Misk** is a real-time embedded operating system (RTOS) designed for desktop productivity devices. Built on **ESP32** and **FreeRTOS**, it uses an object-oriented architecture to manage multiple asynchronous tasks (UI, Networking, NTP Synchronization) ensuring deterministic, robust, and scalable performance.

---

## System Architecture

The system moves away from the classic Arduino "Super Loop" to adopt a modular design based on **Managers**. Each critical subsystem (WiFi, Time, Display) runs in its own FreeRTOS Task, communicating exclusively through an **Event Bus (EventGroup)** to maintain decoupling.

### 1. Class Diagram (OO Design)
Business logic is encapsulated, separating hardware management (`Drivers`) from application logic (`Managers`).

```mermaid
classDiagram
    class Main {
        +setup()
        +loop() (Empty)
    }

    class EventBus {
        <<FreeRTOS EventGroup>>
        +BIT_0: WIFI_CONNECTED
        +BIT_1: TIME_SYNCED
    }

    class WiFiManager {
        -char* ssid
        -char* pass
        +begin()
        +startConnectionTask()
        +isConnected()
        -taskLoop()
    }

    class TimeManager {
        -char* ntpServer
        +getFormattedTime()
        +startTimeTask()
        -taskLoop()
    }

    class DisplayManager {
        -TFT_eSPI tft
        +drawClock()
        +drawStatusIcons()
        +startTask()
        -taskLoop()
    }

    Main --> WiFiManager : Instantiates & Starts
    Main --> TimeManager : Instantiates & Starts
    Main --> DisplayManager : Instantiates & Starts
    WiFiManager ..> EventBus : Publishes State
    TimeManager <..> EventBus : Consumes/Publishes
    DisplayManager <.. EventBus : Consumes State
```
The sequence diagram shows how the system boots up and synchronizes without blocking the main CPU. The UI always remains responsive, even if the network goes down.
```mermaid
    %%{init: {'theme': 'base', 'themeVariables': {'lineColor': '#000000', 'textColor': '#000000', 'signalColor': '#000000', 'actorLineColor': '#000000', 'noteTextColor': '#000000'}}}%%
    sequenceDiagram
    participant Hardware
    participant WiFiTask
    participant EventGroup
    participant TimeTask
    participant UITask

    Note over WiFiTask, UITask: System Startup (Parallel)

    rect rgb(144, 253, 144)
    Note right of WiFiTask: Connection Phase
    WiFiTask->>Hardware: Connect WiFi
    Hardware-->>WiFiTask: IP Obtained
    WiFiTask->>EventGroup: Set BIT_0 (WIFI_READY)
    end

    rect rgb(200, 200, 255)
    Note right of TimeTask: Synchronization Phase
    TimeTask->>EventGroup: Wait for BIT_0
    EventGroup-->>TimeTask: Unblock
    TimeTask->>Hardware: NTP Request (UDP)
    Hardware-->>TimeTask: Epoch Time
    TimeTask->>EventGroup: Set BIT_1 (TIME_SYNCED)
    end

    rect rgb(255, 220, 200)
    loop UI Loop (Non-Blocking)
        UITask->>EventGroup: Read Status Bits
        
        alt System Ready
            UITask->>UITask: Render Clock
        else Loading
            UITask->>UITask: Render "NTP..."
        else Error
            UITask->>UITask: Render "No WiFi"
        end
        
        UITask->>Hardware: SPI Draw (ST7735)
    end
end
```
wiring diagram
```mermaid
    graph LR
    
    subgraph Protoboard [Connection]
        VCC[VCC 3.3V] --- 3V3[3V3]
        GND[GND] --- G[GND]
        CS[CS] --- D5[GPIO 5]
        RST[RES] --- D4[GPIO 4]
        DC[DC] --- D2[GPIO 2]
        SDA[SDA/MOSI] --- D23[GPIO 23]
        SCK[SCK/CLK] --- D18[GPIO 18]
        BL[Backlight] --- D21[GPIO 21]
    end

    ST7735_TFT --- VCC
    ST7735_TFT --- GND
    ST7735_TFT --- CS
    ST7735_TFT --- RST
    ST7735_TFT --- DC
    ST7735_TFT --- SDA
    ST7735_TFT --- SCK
    ST7735_TFT --- BL

    3V3 --- ESP32
    G --- ESP32
    D5 --- ESP32
    D4 --- ESP32
    D2 --- ESP32
    D23 --- ESP32
    D18 --- ESP32
    D21 --- ESP32
```

