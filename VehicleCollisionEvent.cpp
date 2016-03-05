#ifndef VEHICLECOLLISIONEVENT_CPP
#define VEHICLECOLLISIONEVENT_CPP

#include "VehicleCollisionEvent.h"

VehicleCollisionEvent::VehicleCollisionEvent(unsigned ob1, unsigned int ob2, vec3 location, vec3 normal, vec3 force) :CollisionEvent(ob1, ob2, location, normal, force)
{

}

unsigned int VehicleCollisionEvent::getType()
{
	return VEHICLE_COLLISION_EVENT;
}














#endif //COLLISIONEVENT_CPP