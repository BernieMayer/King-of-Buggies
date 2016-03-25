#ifndef POWERUPCOLLISIONEVENT_H
#define POWERUPCOLLISIONEVENT_H

#include "CollisionEvent.h"
#include "glm\glm.hpp"

using namespace glm;
class PowerupCollisionEvent :
	public CollisionEvent
{
public:
	PowerupCollisionEvent(int aPowerupId, int aPlayerId);

	unsigned int getType();
};

#endif