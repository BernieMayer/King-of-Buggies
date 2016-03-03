#ifndef AIMANAGER_H
#define AIMANAGER_H

#include "GameState.h"
#include "LevelInfo.h"
#include "Input.h"
#include "InputSmoother.h"
#include "PxPhysicsAPI.h"
#include "NavMesh.h"
#include <iostream>

using namespace physx;

class AIManager {
public:
	AIManager(GameState* state);

	bool loadNavMesh(const char* fileName);

	int updateCounter;
	PxTransform lastOurTransform;
	PxTransform lastBallTransform;
	PxVec3 rotationalTarget;
	vector<Input> inputs;
	Input updateAI(int playerNum, bool switchType, vec3 pos);
	Input testAIChase(unsigned int aiNum);
	Input testAIEvade(int playerNum);
	void initAI(int pNum);
	void initAI();
	Input driveToPoint(int playerNum, vec3 pos);
	bool atPoint(int playerNum, vec3 pos);

private:
	NavMesh nav;
	vector<unsigned int> playerNums;		//Indices of AI in gameManager
	vector<vector<vec3>> pathToGoal;
	vector<unsigned int> pointOnPath;
	vector<vec3> prevPosition;

	GameState* state;

	//Navigation mesh functions
	void findNewPath(vec3 target);		//Not yet implemented
	void updatePathProgress();			//Not yet implemented


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