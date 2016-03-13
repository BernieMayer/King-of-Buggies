#ifndef COIN_CPP
#define COIN_CPP

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

mat4 Coin::getRotation() {
	mat4 rotation = mat4();
	float cosine = cos(angle);
	float sine = sin(angle);
	rotation[0][0] = cosine;
	rotation[0][2] = sine;
	rotation[2][0] = -sine;
	rotation[2][2] = cosine;

	mat4 oldTransform = this->getTransform();
	return (oldTransform * rotation);
}

#endif //COIN_CPP