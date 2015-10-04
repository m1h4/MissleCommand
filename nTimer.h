// Copyright 2004/2006 Miha Software

#pragma once

class nTimer
{
public:
    nTimer();

    void Reset(); // Resets the timer
    void Start(); // Starts the timer
    void Stop();  // Stop (or pause) the timer
    void Advance(); // Advance the timer by 0.1 seconds
    double GetAbsoluteTime(); // Get the absolute system time
    double GetTime(); // Get the current time
    double GetElapsedTime(); // Get the time that elapsed between GetElapsedTime() calls
    bool IsStopped(); // Returns true if timer stopped

protected:
    bool m_bUsingQPF;
    bool m_bTimerStopped;
    LONGLONG m_llQPFTicksPerSec;

    LONGLONG m_llStopTime;
    LONGLONG m_llLastElapsedTime;
    LONGLONG m_llBaseTime;
};