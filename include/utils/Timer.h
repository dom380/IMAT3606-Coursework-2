#pragma once
#ifndef TIMER_H
#define TIMER_H

#if defined(_WIN32) || defined(_WIN64)   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

/*
	High Resolution (Microsecond) timer for Windows and Unix based systems.
*/
class Timer {
public:
	/*
		Default constructor.
	*/
	Timer();
	~Timer() {};

	/*
		Starts the timer.
	*/
	void start();
	/*
		Stops the timer.
	*/
	void stop();
	/*
		Returns the current elapsed time in seconds.
	*/
	double getElapsedTime();   
	/*
		Returns the current elapsed time in Milliseconds.
	*/
	double getElapsedTimeMilliSec();
	/*
		Returns the current elapsed time in Microseconds.
	*/
	double getElapsedTimeMicroSec();
private:
	double startTimeInMicroSec;                 
	double endTimeInMicroSec;                   
	bool    stopped;                             
#if defined(_WIN32) || defined(_WIN64)
	LARGE_INTEGER frequency;                    // ticks per second
	LARGE_INTEGER startCount;                   
	LARGE_INTEGER endCount;                     
#else
	timeval startCount;                         
	timeval endCount;                           
#endif
};

#endif // !TIMER_H

