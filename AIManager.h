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

enum{
	AI_COMPLETED_PATH,
	AI_IN_PATH
};

class AIManager {
public:
	AIManager(GameState* state);

	//Navigation mesh functions
	bool loadNavMesh(const char* fileName);
	bool findNewPath(unsigned int playerNum, unsigned int targetNum);
	bool findNewPath(unsigned int playerNum, vec3 target);
	unsigned int updatePathProgress(unsigned int playerNum);
	void getPathAsLines(unsigned int playerNum, vector<vec3>* lines);
	vec3 getCurrentTarget(unsigned int playerNum);

	//Test functions
	Input followRandomPath(unsigned int playerNum);
	vec3 getRandomTarget();

	Input getInput(unsigned int playerNum);		//Main interface into ai

	int updateCounter;
	vector<Input> inputs;
	Input updateAI(int playerNum, bool switchType, vec3 pos);
	Input testAIChase(unsigned int aiNum);
	Input testAIEvade(int playerNum);
	void initAI(int pNum);
	void initAI();
	Input driveToPoint(int playerNum, vec3 pos);
	bool atPoint(int playerNum, vec3 pos);

	//Test code - Not for use in final product
	vec3 testTarget;


	NavMesh nav;


private:
	vector<vector<vec3>> pathToGoal;
	vector<unsigned int> pointOnPath;
	vector<vec3> prevPosition;

	GameState* state;

	float lastDot = NULL;
	bool lastTurnL = false;
	float frameCounter = 0;
	GameState lastState;
	bool reversing = false;
	float carSpeed = 0.3f;
	// Default drive to position
	vec3 lastDriveToPos = vec3(-13, 0.5, -15);
	vector<InputSmoother> smoother;		// = InputSmoother();
	float aiType = 0;

	float facing(Entity* object, Entity* target);
	float facing(Entity* object, vec3 targetPos);
	float beside(Entity* object, Entity* target);
	float beside(Entity* object, vec3 targetPos);
};

#endif // AIMANAGER_H