#ifndef EVENTS_H
#define EVENTS_H

#include <limits.h>


const unsigned int NO_VALUE = UINT_MAX;





class Event
{
protected:
	Event();
public:
	virtual unsigned int getType();

};










#endif