#ifndef AIMANAGER_H
#define AIMANAGER_H

#include "GameState.h"
#include "LevelInfo.h"
#include "Input.h"
#include "InputSmoother.h"
#include "PxPhysicsAPI.h"
#include <iostream>

using namespace physx;

class AIManager {
public:
	AIManager();
	int updateCounter;
	PxTransform lastOurTransform;
	PxTransform lastBallTransform;
	PxVec3 rotationalTarget;
	vector<Input> inputs;
	Input updateAI(GameState state, int playerNum, bool switchType, vec3 pos);
	Input testAIChase(GameState state);
	Input testAIEvade(GameState state, int playerNum);
	void initAI(int pNum);
	void initAI(GameState state);
	Input driveToPoint(GameState state, int playerNum, vec3 pos);

private:
	vector<int> playerNums;
	vec3 prevPosition;
	float facing(Entity* object, Entity* target);
	float facing(Entity* object, vec3 targetPos);
	float beside(Entity* object, Entity* target);
	float beside(Entity* object, vec3 targetPos);
	float lastDot = NULL;
	bool lastTurnL = false;
	float frameCounter = 0;
	GameState lastState;
	bool reversing = false;
	float carSpeed = 0.3f;
	// Default drive to position
	vec3 lastDriveToPos = vec3(-13, 0.5, -15);
	InputSmoother smoother = InputSmoother();
	float aiType = 0;
};

#endif // AIMANAGER_H