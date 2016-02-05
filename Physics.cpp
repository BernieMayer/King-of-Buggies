#include "Physics.h"

#include "PxPhysicsAPI.h"

using namespace physx;

Physics::Physics() {
	static PxDefaultErrorCallback gDefaultErrorCallback;

}

void Physics::startSim(GameState state) {

}

GameState Physics::getSim() {
	return GameState();
}

Entity* Physics::getCollisions() {
	return NULL;
}