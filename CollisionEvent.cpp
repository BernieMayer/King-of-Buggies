#ifndef COLLISIONEVENT_CPP
#define COLLISIONEVENT_CPP

#include "CollisionEvent.h"

CollisionEvent::CollisionEvent(unsigned ob1, unsigned int ob2, vec3 location, vec3 normal, vec3 force) : ob1(ob1), ob2(ob2), location(location), normal(normal), force(force)
{

}

unsigned int CollisionEvent::getType()
{
	return COLLISION_EVENT;
}













#endif //COLLISIONEVENT_CPP