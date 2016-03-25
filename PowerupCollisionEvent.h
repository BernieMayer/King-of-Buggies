#ifndef POWERUPCOLLISIONEVENT_H
#define POWERUPCOLLISIONEVENT_H

#include "Events.h"
#include "glm\glm.hpp"

using namespace glm;
class PowerupCollisionEvent :
	public Event
{
public:
	PowerupCollisionEvent(int aPowerupId, int aPlayerId);
	unsigned int getType();
	int powerupId;
	int playerId;
};

#endif