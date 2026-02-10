#ifndef TIMER_H
#define TIMER_H

class Timer{
    public:
        Timer(); //constructor
        
        // Starts the Timer when given a value
        void start(unsigned long durationMs);
        //Resets all values to zero
        void reset();        
        //Detects wether the countdows is running and in case it is not, it resumes it
        void resume();
        //Pauses the Timer if is running
        void pause();
        unsigned long getRemainingTime();        
        //Evaluates if the timer finished
        bool isFinished();
        //return is the counter is running or paused
        bool isRunning();
    
    private:
        //it stores when the Timer started
        unsigned long startTimeMs;
        //it stores the total duration of the Timer
        unsigned long durationMs;
        //it stored how much time was spent before pause
        unsigned long elapsedTimeBeforePause;
        //states if the counter is running or paused
        bool running;
};
#endif