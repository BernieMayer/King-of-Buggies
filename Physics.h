#ifndef PHYSICS_H
#define PHYSICS_H

#include <math.h>
#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"
#include "PxPhysicsAPI.h"
#include "Timer.h"

const float gravity = -20.f;

class Physics {
public:
	Physics();
	void shutdown();
	void startSim(const GameState&);
	GameState getSim();
	Entity* getCollisions();
private:
	Timer clock;

	void initDefaultScene();
	void initVehicle();
};

physx::PxRigidActor* getSphere();	//TEMPORARY FUNCTION

mat4 getMat4(const physx::PxTransform& transform);
vec4 getVec4(const physx::PxVec4& vec);

#endif // PHYSICS_H