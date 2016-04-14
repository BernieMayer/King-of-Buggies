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
	bool findNewPath(unsigned int playerNum, unsigned int targetNum, bool updateOld);
	bool findNewPath(unsigned int playerNum, vec3 target, bool updateOld);
	bool findNewPathConsideringPowerups(unsigned int playerNum, vec3 target, bool updateOld);
	unsigned int updatePathProgress(unsigned int playerNum);
	void getPathAsLines(unsigned int playerNum, vector<vec3>* lines);
	vec3 getCurrentTarget(unsigned int playerNum);

	//Debugging
	bool debugAIPath(vector<vec3>* pathFinding, unsigned int playerNum, unsigned int numIterations);

	//Test functions
	Input followRandomPath(unsigned int playerNum);
	Input followPath(unsigned int playerNum);
	vec3 getRandomTarget();

	Input getInput(unsigned int playerNum, GameState state);
	Input getInput(unsigned int playerNum);		//Main interface into ai

	int updateCounter;
	vector<Input> inputs;
	Input updateAI(int playerNum, bool switchType, vec3 pos);
	Input testAIChase(unsigned int aiNum);
	Input testAIEvade(int playerNum);
	Input recover(int playerNum);
	void initAI(int pNum);
	void initAI();
	Input driveToPoint(int playerNum, vec3 pos);
	bool atPoint(int playerNum, vec3 pos);
	vector<vector<PlayerInfo>> pastInfo;
	void updateRecovery(unsigned int playerNum);

	PlayerInfo infoAtCollision;
	int recoveryThreshold = 60;
	bool collisionRecovery = false;
	int collisionRecoveryCounter = 0;
	// 2 seconds to recover
	int collisionRecoveryCounterMax = 1.5 * 60;
	timeb collisionStartTime;

	//Test code - Not for use in final product
	vec3 testTarget;


	NavMesh nav;

	void clear();


private:
	vector<vector<vec3>> pathToGoal;
	vector<vector<unsigned int> > nodesToGoal;
	vector<unsigned int> pointOnPath;
	vector<vec3> prevPosition;

	GameState* state;

	float lastDot = NULL;
	bool lastTurnL = false;
	float frameCounter = 0;
	GameState lastState;
	vector<bool> reversing;
	float carSpeed = 1.0f;
	// Default drive to position
	vec3 lastDriveToPos = vec3(-13, 0.5, -15);
	int indexOfGoldenBuggy = 0;
	float aiType = 0;
	Timer timer;

	int indexOfClosestPowerup(int playerID);
	float facing(Entity* object, Entity* target);
	float facing(Entity* object, vec3 targetPos);
	float beside(Entity* object, Entity* target);
	float beside(Entity* object, vec3 targetPos);
};

#endif // AIMANAGER_H