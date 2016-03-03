#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"


AIManager::AIManager(GameState* state) :updateCounter(0), state(state), pointOnPath(0)
{

}

bool AIManager::loadNavMesh(const char* fileName)
{
	return nav.loadNavMesh(fileName);
}

void  AIManager::initAI(int pNum) {
	playerNums.push_back(pNum);
}

void  AIManager::initAI() {
	//prevPosition = state->getPlayer(0)->getPos();
	playerNums.clear();

	for (unsigned int i = 0; i < state->numberOfPlayers(); i++)
	{
		if (state->getPlayer(i)->isAI())
		{
			playerNums.push_back(i);
			vector<vec3> path;
			pathToGoal.push_back(path);
			pointOnPath.push_back(0);
			prevPosition.push_back(state->getPlayer(i)->getPos());
		}
	}
}

void AIManager::findNewPath(vec3 target)
{
	
}


/*




*/

Input AIManager::testAIChase(unsigned int aiNum){
	/*
	point vec=hispos-prevpos; 	// vec is the 1-frame position difference
	vec=vec*N; 					// we project N frames into the future
	point futurepos=hispos+vec; // and build the future projection
	reaim(mypos,myyaw,futurepos);
	mypos.x = mypos.x + cos(myyaw) * speed;
	mypos.z = mypos.z + sin(myyaw) * speed;
	*/
	vec3 goldenBuggyLoc = state->getGoldenBuggy()->getPos();
	prevPosition[aiNum] = goldenBuggyLoc;
	vec3 myPos = state->getPlayer(playerNums[aiNum])->getPos();
	vec3 vec = goldenBuggyLoc - prevPosition[aiNum];
	vec = vec * vec3(30, 30, 30); // we project N frames into the future

	vec3 futurepos = goldenBuggyLoc + vec;
	double dX_chaseAI = myPos.x;
	double dY_chaseAI = myPos.y;
	double dZ_chaseAI = myPos.z;

	double yaw_chaseAI = atan2(dZ_chaseAI, dX_chaseAI);

	double dX_golden = goldenBuggyLoc.x;
	double dY_golden = goldenBuggyLoc.y;
	double dZ_golden = goldenBuggyLoc.z;

	double yaw_golden = atan2(dZ_golden, dX_golden);


	Input input = Input();
	input.forward = 0.5f;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;

	Entity* ai = state->getPlayer(playerNums[aiNum]);
	Entity* goldenBuggy = state->getGoldenBuggy();

	float dot = facing(ai, goldenBuggy);
	if (dot < 0.9){
		if (dot > 0){
			float result = beside(ai, goldenBuggy);

			if (result > 0){
				input.turnR = -result;
			}
			else{
				input.turnL = result;
			}

		}
		else {
			float result = beside(ai, goldenBuggy);

			if (result > 0){
				input.turnR = -result;
			}
			else{
				input.turnL = result;
			}

		}
	}

	return smoother.smooth(input);
	
	
}

Input AIManager::updateAI(int playerNum, bool switchType, vec3 pos) {
	if (switchType) {
		if (aiType == 2) {
			aiType = 0;
			cout << "Drive to point\n";
		}
		else if (aiType == 1) {
			aiType += 1;
			cout << "Evade\n";
		}
		else {
			aiType += 1;
			cout << "Chase\n";
		}
	}

	if (aiType == 0) {
		return driveToPoint(playerNum, pos);
	}
	else if (aiType == 1) {
		return testAIChase(playerNum);
	}
	else {
		return testAIEvade(playerNum);
	}
}

// If result is negative, ai is facing away from player
// If result is positive ai is facing player
// Returns value between -1 and 1
float AIManager::facing(Entity* object, Entity* target) {
	vec3 targetPos = target->getPos();
	
	return facing(object, targetPos);
}

float AIManager::facing(Entity* object, vec3 targetPos) {
	vec3 objectPos = object->getPos();
	vec3 objectForward = object->getForward();
	objectForward = normalize(objectForward);

	vec3 difference = targetPos - objectPos;
	difference = normalize(difference);
	float result = dot(difference, objectForward);

	return result;
}

// Returns positive if target is to the right of object
// Negative for left
// Returns a value between -1 and 1
float AIManager::beside(Entity* object, Entity* target) {
	vec3 targetPos = target->getPos();
	
	return beside(object, targetPos);
}

float AIManager::beside(Entity* object, vec3 targetPos) {
	vec3 objectPos = object->getPos();
	vec3 objectForward = object->getForward();
	vec3 objectUp = object->getUp();
	vec3 objectSide = cross(objectForward, objectUp);
	objectSide = normalize(objectSide);

	vec3 difference = targetPos - objectPos;
	difference = normalize(difference);
	float result = dot(difference, objectSide);

	return result;
}

Input AIManager::testAIEvade(int playerNum) {	
	Entity* ai = state->getPlayer(playerNums[playerNum]);
	vec3 aiPos = ai->getPos();

	Entity* player = NULL;
	float shortestLength = 0;
	for (unsigned int i = 0; i < state->numberOfPlayers(); i++) {
		if (i != playerNum) {
			Entity* indexedPlayer = state->getPlayer(i);
			vec3 playerPos = indexedPlayer->getPos();
			vec3 distance = playerPos - aiPos;

			float len = length(distance);
			if (len < shortestLength || player == NULL) {
				shortestLength = len;
				player = indexedPlayer;
			}
		}
	}
	
	Input input = Input();
	input.forward = carSpeed;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;

	float dot = facing(ai, player);
	float side = beside(ai, player);


	float distance = length(aiPos - player->getPos());
	float speed = ((PlayerInfo*)ai)->getFSpeed();
	
	// If facing car and very close
	if (dot >= 0.8 && distance <= 10) {
		// Go backwards
		input.forward = 0.0f;
		input.backward = carSpeed;
		// if still moving forwards
		if (!reversing && speed > 0) {
			// Do not turn as direction of turning will be changing soon
			input.turnL = 0;
			input.turnR = 0;
		}
		// If moving backwards
		else if ((!reversing && speed <= 0) || reversing) {
			reversing = true;
				// Turn
			if (side > 0) {
				input.turnL = dot;
			}
			else {
				input.turnR = dot;
			}
		}
	}
	else if (dot > -0.9) {
		// If still moving backwards, do not turn as turning direction will be changing soon
		if (reversing && speed < 0) {
			input.turnL = 0;
			input.turnR = 0;
		}
		// If moving forwards
		else if ((reversing && speed >= 0) || !reversing) {
			reversing = false;
				// Turn
			if (side > 0) {
				input.turnR = side;
			}
			else {
				input.turnL = -side;
			}
		}
	}

	prevPosition[playerNum] = aiPos;

	return smoother.smooth(input);
}

Input AIManager::driveToPoint(int playerNum, vec3 pos) {
	lastDriveToPos = pos;

	
	
	Entity* ai = state->getPlayer(playerNums[playerNum]);
	vec3 aiPos = ai->getPos();

	Input input = Input();
	input.forward = carSpeed;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;

	float dot = facing(ai, pos);
	float side = beside(ai, pos);

	// If not facing towards point
	if (dot < 0.95f) {
		// Turn
		if (side < 0) {
			input.turnR = 1;
		}
		else {
			input.turnL = 1;
		}
	}

	prevPosition[playerNum] = aiPos;

	return smoother.smooth(input);
}

bool AIManager::atPoint(int playerNum, vec3 pos) {
	PlayerInfo* ai = state->getPlayer(playerNum);

	float distance = length(ai->getPos() - pos);
	if (distance < 3) {
		return true;
	}
	else {
		 return false;
	}
}

#endif
