#include "PowerupBox.h"

mat4 PowerupBox::getRotation(float angle) {
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

bool PowerupBox::getCollided() { return collided; }

void PowerupBox::setCollided(bool isCollided) { collided = isCollided; }


void PowerupBox::startCountdown() {
	countdown = 600;
}

void PowerupBox::decrementCountdown() {
	countdown--;
}

int PowerupBox::getCountdown() {
	return countdown;
}
