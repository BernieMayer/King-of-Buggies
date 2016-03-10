#ifndef VEHICLEWALLCOLLISIONEVENT_CPP
#define VEHICLEWALLCOLLISIONEVENT_CPP

#include "VehicleWallCollisionEvent.h"


VehicleWallCollisionEvent::VehicleWallCollisionEvent(unsigned ob1, vec3 location, vec3 normal, vec3 force): CollisionEvent(ob1, -1, location, normal, force)
{

}

unsigned int VehicleWallCollisionEvent::getType()
{
	return VEHICLE_WALL_COLLISION_EVENT;
}

#endif

