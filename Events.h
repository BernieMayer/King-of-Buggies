#ifndef EVENTS_H
#define EVENTS_H

#include <limits.h>


const unsigned int NO_VALUE = UINT_MAX;

//List all subclass collision types here
enum
{
	COLLISION_EVENT,
	VEHICLE_COLLISION_EVENT,
};



class Event
{
protected:
	Event();
public:
	virtual unsigned int getType();

};










#endif