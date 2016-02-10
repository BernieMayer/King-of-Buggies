#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <sys/timeb.h>

class Timer {
private:
	struct timeb previous;

public:
	Timer();

	float getElapsedTime();		//In seconds

};

#endif // TIMER_H