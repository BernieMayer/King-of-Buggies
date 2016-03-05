#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "Events.h"
#include <glm/glm.hpp>

using namespace glm;

class CollisionEvent : public Event
{
private:

public:
	unsigned int ob1, ob2, type;
	vec3 location, normal, force;

	CollisionEvent(unsigned ob1, unsigned int ob2, vec3 location, vec3 normal, vec3 force);

	unsigned int getType();

};














#endif