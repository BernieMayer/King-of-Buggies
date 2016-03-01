#include "Coin.h"

void Coin::setCollided(bool hasCollision) {
	collided = hasCollision;
}

bool Coin::isCollided() {
	return collided;
}

void Coin::startCountdown() {
	countdown = 200;
}

void Coin::decrementCountdown() {
	countdown--;
}

int Coin::getCountdown() {
	return countdown;
}