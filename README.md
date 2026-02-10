# Misk: Advanced Desktop Productivity OS

![Status](https://img.shields.io/badge/Status-Alpha-yellow)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino%20%7C%20FreeRTOS-green)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

**Misk** is a real-time embedded operating system (RTOS) designed for desktop productivity devices. Built on **ESP32** and **FreeRTOS**, it uses an object-oriented architecture to manage multiple asynchronous tasks (UI, Networking, NTP Synchronization, apps such as Pomodoro timers, Clock,etc.) ensuring deterministic, robust, and scalable performance.

**NOTE THAT THIS IS AN EDUCATIONAL PROJECT MADE FOR PERSONAL USE AND ITS FINAL PURPOSE IS FOR LEARNING**

---

## System Architecture

The system tries to move away as much from the classic Arduino "Super Loop" to adopt a modular design based on **Managers**. Each critical subsystem (WiFi, Time, Display) runs in its own FreeRTOS Task, communicating exclusively through an **Event Bus (EventGroup)** to maintain decoupling.

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
### 2. Sequence Diagram
The system boots up and synchronizes without blocking the main CPU. The UI always remains responsive, even if the network goes down.
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
### Wiring diagram - ESP32 and ST7735 TFT 1,8"(160*128) 
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
## Input Manager
For the input we use a constant polling(20ms) due to the complications of using interruptions just for the buttons,
```mermaid
    sequenceDiagram
    participant button
    participant inputManager
    participant app

    app ->> inputManager: actionBasedOnInput(button.wasPresed())
    inputManager ->> button: wasPressed()
    button-->>inputManager: true/false
    
    inputManager -->> app: execute actionBasedOnInput()

```
## Pomodoro Manager
The PomodoroManager has to synchronize depending on the state you are on and your configuration of rounds and time for each type of round. 
```mermaid
   stateDiagram-v2
    
    classDef def fill:#f9f9f9,stroke:#333,stroke-width:2px;
    classDef decision fill:a52019, stroke:#a52019,stroke-width:2px;
    classDef action fill:#90caf9,stroke:#1565c0,stroke-width:2px;

    [*] --> IDLE

    IDLE --> FOCUS :  Start button

    state FOCUS {
        [*] --> Working
        Working --> PAUSE : Pause button
        PAUSE --> Working : Resume button
    }

    FOCUS --> CheckRounds() : Time == 0

    state CheckRounds() <<choice>>
    class CheckRounds() decision
    state WAITING_FOR_INPUT 
    
    CheckRounds() --> WAITING_FOR_INPUT
    WAITING_FOR_INPUT --> SHORT_BREAK : Rounds < objective
    WAITING_FOR_INPUT --> LONG_BREAK : Rounds == objective

    state SHORT_BREAK {
        [*] --> RestingShort
        RestingShort --> PAUSE_BREAK : Pause button
        PAUSE_BREAK --> RestingShort : Resume button
    }

    state LONG_BREAK {
        [*] --> RestingLong
        RestingLong --> PAUSE_LONG : Pause button
        PAUSE_LONG --> RestingLong : Resume button
    }

    %% Lógica de retorno
    SHORT_BREAK --> WAITING_FOR_INPUT : Time == 0 
    WAITING_FOR_INPUT-->FOCUS : Rounds < objective
    LONG_BREAK --> IDLE : Time == 0 (Final session)
    
```