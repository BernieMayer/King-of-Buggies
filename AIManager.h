#ifndef AIMANAGER_H
#define AIMANAGER_H

#include "GameState.h"
#include "LevelInfo.h"
#include "Input.h"
#include "PxPhysicsAPI.h"
#include <iostream>

using namespace physx;

class AIManager {
public:

	int updateCounter = 0;
	PxTransform lastOurTransform;
	PxTransform lastBallTransform;
	PxVec3 rotationalTarget;
	vector<Input> inputs;
	Input updateAI(GameState* state);
	Input testAI(PxTransform ourTransform, PxTransform ballTransform);
	void initAI();
};

#endif // AIMANAGER_H