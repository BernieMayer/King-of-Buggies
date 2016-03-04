#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "Events.h"
#include <glm/glm.hpp>

using namespace glm;

enum {
	COLLISION_EVENT = 0,
	VEHICLE_BOX_COLLISION_EVENT,
	VEHICLE_COIN_COLLISION_EVENT,
	VEHICLE_OBSTACLE_COLLISION_EVENT
};

class CollisionEvent : public Event
{
private:

public:
	unsigned int ob1, ob2, type;
	vec3 location, normal, force;

	CollisionEvent(unsigned int type, unsigned ob1, unsigned int ob2, vec3 location, vec3 normal, vec3 force);

	unsigned int getType();

};














#endif