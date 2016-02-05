#ifndef PHYSICS_H
#define PHYSICS_H

#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"

class Physics {
public:
	Physics();
	void shutdown();
	void startSim(GameState);
	GameState getSim();
	Entity* getCollisions();
private:
	void initDefaultScene();
};

#endif // PHYSICS_H