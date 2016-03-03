#ifndef COIN_H
#define COIN_H

#include "Entity.h"

class Coin: public Entity {
public:
	Coin() : collided(false), angle(.20f) {};

	void setCollided(bool hasCollision);
	bool isCollided();

	void startCountdown();
	void decrementCountdown();
	int getCountdown();

	mat4 getRotation();

private:
	bool collided;
	int countdown;
	float angle;
};

#endif //COIN_H