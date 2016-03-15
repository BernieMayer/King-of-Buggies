#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <Windows.h>
#include <sys/timeb.h>
#include <cstdio>

class Timer {
private:
	struct timeb previous;

public:
	Timer();

	void start();
	float getElapsedTime();		//In seconds
	void waitUntil(float targetTime);
	timeb getCurrentTime();
	float getTimeSince(timeb previousTime);


};

#endif // TIMER_H