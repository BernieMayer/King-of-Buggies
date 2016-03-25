#ifndef POWERUPBOX_H
#define POWERUPBOX_H

#include "Entity.h"

class PowerupBox: public Entity {
public:
	PowerupBox() {};
	mat4 getRotation(float angle);
};

#endif //POWERUPBOX_H