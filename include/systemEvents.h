#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

//here we use bitwise operations to keep track of different
//modules of the device that need to be actiavted or not for certain
//actions to happen

/*The << (left shift) in C takes two numbers, the left 
shifts the bits of the first operand, and the second operand 
decides the number of places to shift. */

#define WIFI_CONNECTED_BIT (1 << 0) // 0000 0001

#define TIME_SYNCED_BIT (1 << 1) // 0000 0010

//next would be (1 << 2) -> 0000 0100
//

enum class InputEvent {
    BTN_ACTION_SHORT, // Click (Start/Pause)
    BTN_ACTION_LONG,  // Hold (Quizás apagar pantalla?)
    
    BTN_RESET_SHORT,  // Click (Stop/Reset)
    BTN_RESET_LONG,   // Hold (Volver a Clock?)
    
    BTN_SKIP_SHORT,
    BTN_SKIP_LONG,   // Click (Saltar)
    
    BTN_MODE_SHORT, // Botón 1 (Cambiar App)
    BTN_MODE_LONG
};

// El mensaje que viajará por la cola
struct SystemCommand {
    InputEvent type;
    // Podríamos añadir datos extra, ej: uint32_t value;
};

#endif