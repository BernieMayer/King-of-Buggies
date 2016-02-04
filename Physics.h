#ifndef PHYSICS_H
#define PHYSICS_H

#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"

class Physics {
public:
	Physics();
	void startSim(GameState);
	GameState getSim();
	Entity* getCollisions();
};

#endif // PHYSICS_H