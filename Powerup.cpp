#ifndef POWERUP_CPP
#define POWERUP_CPP

#include "Powerup.h"

void Powerup::setTransform(const mat4& _transform, float time)
{
	vec3 originalPos = getPos();
	transform = _transform;
	velocity = (getPos() - originalPos) / time;
}

vec3 Powerup::getVelocity() { return velocity; }






#endif //POWERUP_CPP