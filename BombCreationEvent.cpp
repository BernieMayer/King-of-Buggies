#ifndef BOMBCREATIONEVENT_CPP
#define BOMBCREATIONEVENT_CPP

#include "BombCreationEvent.h"


BombCreationEvent::BombCreationEvent(vec3 location)
{
	pos = location;
}

unsigned int BombCreationEvent::getType() {
	return BOMB_CREATION_EVENT;
}

#endif

