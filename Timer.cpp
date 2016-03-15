#ifndef TIMER_CPP
#define TIMER_CPP

#include "Timer.h"

Timer::Timer()
{
	start();
}

void Timer::start()
{
	ftime(&previous);
}



float Timer::getElapsedTime()
{
	timeb current;
	ftime(&current);

	int diff = (int)(1000.0 * (current.time - previous.time)
		+ (previous.millitm - previous.millitm));

	previous = current;
	return ((float)diff)*0.001f;
}

timeb Timer::getCurrentTime()
{
	timeb current;
	ftime(&current);

	return current;
}

float Timer::getTimeSince(timeb previousTime)
{
	timeb current;
	ftime(&current);

	int diff = (int)(1000.0 * (current.time - previousTime.time)
		+ (previousTime.millitm - previousTime.millitm));

	return ((float)diff)*0.001f;
}

void Timer::waitUntil(float targetTime)
{
	Sleep(targetTime*1000.f);
}


#endif // TIMER_CPP