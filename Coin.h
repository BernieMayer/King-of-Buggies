#ifndef COIN_H
#define COIN_H

#include "Entity.h"
#include "math.h"

class Coin: public Entity {
public:
	Coin() : collided(false){};

	void setCollided(bool hasCollision);
	bool isCollided();

	void startCountdown();
	void decrementCountdown();
	int getCountdown();

	mat4 getRotation(float angle);
	
private:
	bool collided;
	int countdown;
	
};

#endif //COIN_H