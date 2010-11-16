/************************************************************************************
 *	High Resolution Windows Timer Controller
 *	----------------------------------------
 *	code by : bobby anguelov - banguelov@cs.up.ac.za
 *	downloaded from : takinginitiative.wordpress.org
 *
 *	code is free for use in whatever way you want, however if you work for a game
 *	development firm you are obliged to offer me a job :P (haha i wish)
 ************************************************************************************/
#ifndef HRTIMER
#define HRTIMER

#include <windows.h>
#include <vector>

class HRTimer
{
private:	
	
	LARGE_INTEGER start;
    LARGE_INTEGER stop;
	LARGE_INTEGER frequency;
	LARGE_INTEGER timeElapsed;

public:

	//constructor
	HRTimer( LARGE_INTEGER f ) : frequency(f) {}
	
	void reset() { QueryPerformanceCounter( &start ); }
	void startTimer() { reset(); }	//shortcut for readability

	//return elapsed time in seconds
    double getElapsedTimeSeconds()
	{	
		QueryPerformanceCounter( &stop );			
		timeElapsed.QuadPart = stop.QuadPart - start.QuadPart;			
		return (double) timeElapsed.QuadPart / frequency.QuadPart ;		
	}
};

class HRTimerController
{

private:

    LARGE_INTEGER frequency;
	std::vector<HRTimer> timers;

public:
    
	HRTimerController()
	{
		//get CPU frequency once - assuming CPU speed doesnt change during run-time
		QueryPerformanceFrequency( &frequency );
	}

    void createTimer()
	{
		timers.push_back( HRTimer(frequency) );		
	}

	HRTimer* operator[](int n)
	{
		if ( n < (int) timers.size() ) return &timers[n];
		else return NULL;
	}

	int getNumTimers()
	{
		return timers.size();
	}
};

#endif
