#ifndef POWERUPCOLLISIONEVENT_CPP
#define POWERUPCOLLISIONEVENT_CPP

#include "PowerupCollisionEvent.h"

PowerupCollisionEvent::PowerupCollisionEvent(int aPlayerId, int aPowerupId) :CollisionEvent(aPlayerId, aPowerupId, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0))
{

}

unsigned int PowerupCollisionEvent::getType() {
	return POWERUPBOX_COLLISION_EVENT;
}

#endif