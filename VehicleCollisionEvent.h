#ifndef VEHICLECOLLISIONEVENT_H
#define VEHICLECOLLISIONEVENT_H

#include "CollisionEvent.h"
#include <glm/glm.hpp>

using namespace glm;

class VehicleCollisionEvent : public CollisionEvent
{
private:

public:
	VehicleCollisionEvent(unsigned ob1, unsigned int ob2, vec3 location, vec3 normal, vec3 force);

	unsigned int getType();

};



#endif	//VEHICLECOLLISIONEVENT_H