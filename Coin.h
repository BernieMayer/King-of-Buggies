#ifndef COIN_H
#define COIN_H

#include "Entity.h"

class Coin: public Entity {
public:
	Coin() : collided(false) {};

	void setCollided(bool hasCollision);
	bool getCollided();

private:
	bool collided;
};

#endif //COIN_H