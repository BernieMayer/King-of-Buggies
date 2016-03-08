#ifndef VEHICLEWALLCOLLISIONEVENT_H
#define VEHICLEWALLCOLLISIONEVENT_H


#include "CollisionEvent.h"
class VehicleWallCollisionEvent :
	public CollisionEvent
{
public:
	VehicleWallCollisionEvent(unsigned ob1, vec3 location, vec3 normal, vec3 force);

	unsigned int getType();
};

#endif

