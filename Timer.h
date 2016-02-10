#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <Windows.h>
#include <sys/timeb.h>

class Timer {
private:
	struct timeb previous;

public:
	Timer();

	float getElapsedTime();		//In seconds
	void waitUntil(float targetTime);		//Don't use! Hogs CPU

};

#endif // TIMER_H