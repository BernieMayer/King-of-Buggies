#ifndef POWERUPCOLLISIONEVENT_CPP
#define POWERUPCOLLISIONEVENT_CPP

#include "PowerupCollisionEvent.h"

PowerupCollisionEvent::PowerupCollisionEvent(int aPowerupId, int aPlayerId)
{
	powerupId = aPowerupId;
	playerId = aPlayerId;
}

unsigned int PowerupCollisionEvent::getType() {
	return POWERUP_COLLISION_EVENT;
}

#endif