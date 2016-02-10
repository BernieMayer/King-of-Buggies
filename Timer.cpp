#ifndef TIMER_CPP
#define TIMER_CPP

#include "Timer.h"

Timer::Timer()
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

void Timer::waitUntil(float targetTime)
{
	float timeElapsed = 0.f;
	while (timeElapsed < targetTime)
	{
		timeElapsed += getElapsedTime();
	}
}


#endif // TIMER_CPP