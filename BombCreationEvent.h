#ifndef BOMBCREATIONEVENT_H
#define BOMBCREATIONEVENT_H

#include "Events.h"
#include "glm\glm.hpp"

using namespace glm;
class BombCreationEvent :
	public Event
{
public:
	BombCreationEvent(vec3 location);
	unsigned int getType();
	vec3 pos;
};

#endif

