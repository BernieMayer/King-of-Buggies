#ifndef POWERUPBOX_H
#define POWERUPBOX_H

#include "Entity.h"

class PowerupBox: public Entity {
public:
	PowerupBox() : collided(false) {};
	mat4 getRotation(float angle);

	bool getCollided();
	void setCollided(bool isCollided);

	void startCountdown();
	void decrementCountdown();
	int getCountdown();

private:
	bool collided;
	int countdown;
};

#endif //POWERUPBOX_H