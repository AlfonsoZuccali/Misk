#include "Timer.h"
#include <Arduino.h>

Timer :: Timer(){
    //when starting the Timer, everything is set to 0
    reset(); 
}

void Timer :: start(unsigned long durationMs){
    //to avoid starting when it already has
    if(isFinished() == true || running == false){
        this->durationMs = durationMs;
        startTimeMs = millis();
        elapsedTimeBeforePause = 0;
        running = true;
    }
}

void Timer :: reset(){
    startTimeMs = 0;
    durationMs = 0;
    elapsedTimeBeforePause = 0;
    running = false;
}

void Timer :: resume(){
    //you can only resume if it is paused
    if(running == false){
        //the difference between the two points will return when was the last
        //moment before pause
        startTimeMs = millis() - elapsedTimeBeforePause;
        running = true;
    }
}

void Timer :: pause(){
    // Record the time already elapsed since the Timer started:
    elapsedTimeBeforePause = millis() - startTimeMs;
    // Mark the Timer as not running so future calls know it's paused.
    running = false;
}

unsigned long Timer :: getRemainingTime(){
    // If the Timer is currently running, calculate remaining time as:
    // duration minus time elapsed since start.
    if(running){
        return durationMs - (millis() - startTimeMs);
    
    }
    // If the Timer is paused
    return durationMs - elapsedTimeBeforePause;
}

bool Timer :: isFinished(){
    // Return true when the elapsed time since the (adjusted) start time
    // is greater than or equal to the configured duration.
    if(running){
        return (millis() - startTimeMs) >= durationMs;
    }else{
        return false;
    }
}

bool Timer :: isRunning(){
    return running;
}

