#ifndef VEHICLEBOMBCOLLISIONEVENT_CPP
#define VEHICLEBOMBCOLLISIONEVENT_CPP

#include "VehicleBombCollisionEvent.h"

VehicleBombCollisionEvent::VehicleBombCollisionEvent(unsigned vehicle, unsigned int bomb, vec3 location) :CollisionEvent(vehicle, bomb, location, vec3(0, 0, 0), vec3(0, 0, 0))
{

}

unsigned int VehicleBombCollisionEvent::getType()
{
	return VEHICLE_BOMB_COLLISION_EVENT;
}














#endif //COLLISIONEVENT_CPP
