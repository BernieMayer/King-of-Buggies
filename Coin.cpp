#include "Coin.h"

void Coin::setCollided(bool hasCollision) {
	collided = hasCollision;
}

bool Coin::getCollided() {
	return collided;
}