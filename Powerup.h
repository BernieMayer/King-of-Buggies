#ifndef POWERUP_H
#define POWERUP_H

#include "Entity.h"

class Powerup : public Entity {
	int type;
	vec3 velocity;

public:
	Powerup() {};

	void setTransform(const mat4& transform, float time);
	vec3 getVelocity();
};

#endif //POWERUP_H