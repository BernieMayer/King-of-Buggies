#ifndef VEHICLEBOMBCOLLISIONEVENT_H
#define VEHICLEBOMBCOLLISIONEVENT_H

#include "CollisionEvent.h"
#include <glm/glm.hpp>

using namespace glm;

class VehicleBombCollisionEvent : public CollisionEvent
{
private:

public:
	VehicleBombCollisionEvent(unsigned ob1, unsigned int ob2, vec3 location);

	unsigned int getType();

};



#endif	//VEHICLEBOMBCOLLISIONEVENT_H