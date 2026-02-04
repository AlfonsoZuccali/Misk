#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

//here we use bitwise operations to keep track of different
//modules of the device

/*The << (left shift) in C takes two numbers, the left 
shifts the bits of the first operand, and the second operand 
decides the number of places to shift. */

#define WIFI_CONNECTED_BIT (1 << 0) // 0000 0001

#define TIME_SYNCED_BIT (1 << 1) // 0000 0010

//next would be (1 << 2) -> 0000 0100
//

#endif