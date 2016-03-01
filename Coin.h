#ifndef COIN_H
#define COIN_H

#include "Entity.h"

class Coin: public Entity {
public:
	Coin() : collided(false) {};

	void setCollided(bool hasCollision);
	bool isCollided();

	void startCountdown();
	void decrementCountdown();
	int getCountdown();

private:
	bool collided;
	int countdown;
};

#endif //COIN_H