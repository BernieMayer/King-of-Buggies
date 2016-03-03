#ifndef EVENTS_H
#define EVENTS_H

#include <limits.h>

enum {
	VEHICLE_VEHICLE_COLLISION_EVENT,
	VEHICLE_BOX_COLLISION_EVENT,
	VEHICLE_COIN_COLLISION_EVENT,
	VEHICLE_OBSTACLE_COLLISION_EVENT
};


const unsigned int NO_OBJECT = UINT_MAX;





class Event
{
	Event(unsigned int type, unsigned int object1, unsigned int object2);

	unsigned int type, object1, object2;
};










#endif