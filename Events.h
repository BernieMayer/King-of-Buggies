#ifndef EVENTS_H
#define EVENTS_H

#include <limits.h>


const unsigned int NO_VALUE = UINT_MAX;

//List all subclass collision types here
enum
{
	COLLISION_EVENT,
	VEHICLE_COLLISION_EVENT,
	VEHICLE_WALL_COLLISION_EVENT,
	GOLDEN_BUGGY_SWITCH_EVENT,
	BOMB_CREATION_EVENT,
	VEHICLE_BOMB_COLLISION_EVENT,
	POWERUPBOX_COLLISION_EVENT,
};



class Event
{
protected:
	Event();
public:
	virtual unsigned int getType();

};










#endif