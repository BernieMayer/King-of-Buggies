#ifndef PHYSICS_H
#define PHYSICS_H

#include "MeshObject.h"
#include "LevelInfo.h"

class Physics {
public:
	void startSim(GameState);
	GameState getSim();
	Entity* getCollisions();
};

#endif // PHYSICS_H