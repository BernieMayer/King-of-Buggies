#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"


AIManager::AIManager(GameState* state) :updateCounter(0), state(state), pointOnPath(0)
{
	nav.state = state;
}

bool AIManager::loadNavMesh(const char* fileName)
{
	return nav.loadNavMesh(fileName);
}

void  AIManager::initAI(int pNum) {
	
}

void  AIManager::initAI() {
	//prevPosition = state->getPlayer(0)->getPos();
	timer.start();
	for (unsigned int i = 0; i < state->numberOfPlayers(); i++)
	{
		vector<vec3> path;
		vector<unsigned int> nodes;
		pathToGoal.push_back(path);
		nodesToGoal.push_back(nodes);
		pointOnPath.push_back(0);
		prevPosition.push_back(state->getPlayer(i)->getPos());
		reversing.push_back(false);
		vector<PlayerInfo> temp;
		temp.push_back(*state->getPlayer(i));
		pastInfo.push_back(temp);
	}
}

bool AIManager::findNewPath(unsigned int playerNum, unsigned int targetNum, bool updateOld)
{
	return findNewPath(playerNum, state->getPlayer(targetNum)->getPos(), updateOld);
}

bool AIManager::findNewPath(unsigned int playerNum, vec3 target, bool updateOld)
{
	pathToGoal[playerNum].clear();
	pointOnPath[playerNum] = 0;
	return nav.getPathPoints(&pathToGoal[playerNum], &nodesToGoal[playerNum], state->getPlayer(playerNum)->getPos(), state->getPlayer(playerNum)->getForward(), target, updateOld);
}

bool AIManager::findNewPathConsideringPowerups(unsigned int playerNum, vec3 target, bool updateOld)
{
	pathToGoal[playerNum].clear();
	pointOnPath[playerNum] = 0;
	/*
	
		if the distance to a powerup is less than some constant the path should be towards the powerup
	*/
	return nav.getPathPoints(&pathToGoal[playerNum], &nodesToGoal[playerNum], state->getPlayer(playerNum)->getPos(), state->getPlayer(playerNum)->getForward(), target, updateOld);


}

bool AIManager::debugAIPath(vector<vec3>* pathFinding, unsigned int playerNum, unsigned int numIterations)
{
	pathFinding->clear();
	return nav.debugAvoidance(pathFinding, &nodesToGoal[playerNum], state->getPlayer(playerNum)->getPos(), state->getPlayer(playerNum)->getForward(), numIterations);
}

vec3 AIManager::getRandomTarget()
{
	return nav[nav.getRandomNode()].getCenter();
}

vec3 AIManager::getCurrentTarget(unsigned int playerNum)
{
	return pathToGoal[playerNum][pointOnPath[playerNum]];
}


unsigned int AIManager::updatePathProgress(unsigned int playerNum)
{
	while ((pointOnPath[playerNum] < pathToGoal[playerNum].size()) &&
		(atPoint(playerNum, pathToGoal[playerNum][pointOnPath[playerNum]])))
	{
		pointOnPath[playerNum]++;
	}
	
	if (pointOnPath[playerNum] >= pathToGoal[playerNum].size())
		return AI_COMPLETED_PATH;
	else
		return AI_IN_PATH;
}

Input AIManager::followRandomPath(unsigned int playerNum)
{
	if (updatePathProgress(playerNum) == AI_COMPLETED_PATH)
	{

		bool invalidPoint = true;

		while (invalidPoint)
		{
			invalidPoint = false;

			testTarget = getRandomTarget();

			for (unsigned int i = 0; i < state->numberOfPlayers(); i++)
			{
				vec3 playerLoc = state->getPlayer(i)->getPos();;

				if (length(testTarget - playerLoc) < 5.f)
				{
					invalidPoint = true;
					break;
				}
			}
		}

		vec3 pos = state->getPlayer(playerNum)->getPos();

		findNewPath(playerNum, testTarget, true);
	}
	if (collisionRecovery)
		return recover(playerNum);

	else if (pathToGoal[playerNum].size() > 0)
		return driveToPoint(playerNum, pathToGoal[playerNum][pointOnPath[playerNum]]);
	else
		return Input();
}

Input AIManager::followPath(unsigned int playerNum)
{
	if (collisionRecovery)
		return recover(playerNum);
	else if (pathToGoal[playerNum].size() > 0)
		if ((pathToGoal[playerNum].size() <= 2 || pathToGoal.size() >= 1) && playerNum != indexOfGoldenBuggy)
			return testAIChase(playerNum);
		else 
		return driveToPoint(playerNum, pathToGoal[playerNum][pointOnPath[playerNum]]);
	else
		return Input();
}

void AIManager::getPathAsLines(unsigned int playerNum, vector<vec3>* lines)
{
	for (unsigned int i = 1; i < pathToGoal[playerNum].size(); i++)
	{
		lines->push_back(pathToGoal[playerNum][i - 1]);
		lines->push_back(pathToGoal[playerNum][i]);
	}
}

Input AIManager::getInput(unsigned int playerNum, GameState state)
{
	indexOfGoldenBuggy = state.getGoldenBuggyID();

	return getInput(playerNum);

}

Input AIManager::getInput(unsigned int playerNum)
{
	updateRecovery(playerNum);

	return followPath(playerNum);
	//return followRandomPath(playerNum);		//Test function
}


/*




*/

Input AIManager::testAIChase(unsigned int aiNum){
	/*
	Notes about AI stuff
	point vec=hispos-prevpos; 	// vec is the 1-frame position difference
	vec=vec*N; 					// we project N frames into the future
	point futurepos=hispos+vec; // and build the future projection
	reaim(mypos,myyaw,futurepos);
	mypos.x = mypos.x + cos(myyaw) * speed;
	mypos.z = mypos.z + sin(myyaw) * speed;
	*/

	vec3 goldenBuggyLoc = state->getGoldenBuggy()->getPos();
	prevPosition[aiNum] = goldenBuggyLoc;
	vec3 myPos = state->getPlayer(aiNum)->getPos();
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
	input.tiltBackward = 0;
	input.tiltForward = 0;
	input.spinL = 0;
	input.spinR = 0;
	input.rollL = 0;
	input.rollR = 0;

	

	Entity* ai = state->getPlayer(aiNum);
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

	if (state->getPlayer(aiNum)->getCurrentPowerup() != -1)
	{
		if (state->getPlayer(aiNum)->getCurrentPowerup() != POWERUPS::BOMB)
		{
			input.powerup = true;
		}
		else
		{
			
			double dist = abs(length(state->getPlayer(aiNum)->getPos() - state->getGoldenBuggy()->getPos()));

			if (dist <= 5)
			{
				//Use the bomb powerup if the AI is close to the golden buggy
				input.powerup = true;
			}
		}
	}

	return input;
	
	
}

Input AIManager::recover(int playerNum) {
	// If recovery just started
	if (collisionRecoveryCounter == 0) {
		// Reverse direction
		reversing[playerNum] = !reversing[playerNum];
	}

	

	Input input = Input();
	if (reversing[playerNum]) {
		input.forward = 0;
		input.backward = carSpeed;
	}
	else {
		input.forward = carSpeed;
		input.backward = 0;
	}

	input.turnL = 0;
	input.turnR = 0;
	input.tiltBackward = 0;
	input.tiltForward = 0;
	input.spinL = 0;
	input.spinR = 0;
	input.rollL = 0;
	input.rollR = 0;

	float dot = facing(state->getPlayer(playerNum), infoAtCollision.getPos());
	float side = beside(state->getPlayer(playerNum), infoAtCollision.getPos());

	if (dot > 1) {
		dot = 1.0f;
	}
	else if (dot < -1) {
		dot = -1;
	}
	else if (dot != dot) {
		dot = 1;
	}

	if (side > 1) {
		side = 1.0f;
	}
	else if (side < -1) {
		side = -1;
	}
	else if (side != side) {
		side = 1;
	}

	if (reversing[playerNum]) {
		if (side > 0) {
			input.turnL = dot;
		}
		else {
			input.turnR = dot;
		}
	}
	else {
		if (side > 0) {
			input.turnR = dot;
		}
		else {
			input.turnL = dot;
		}
	}

	if (timer.getTimeSince(collisionStartTime) >= 0.9f) {
		input.jump = true;
	}

	return input;
}

void AIManager::updateRecovery(unsigned int playerNum)
{
	// Keeps a sort of cyclical vector
	if (pastInfo[playerNum].size() < recoveryThreshold) {
		pastInfo[playerNum].erase(pastInfo[playerNum].begin());
	}
	pastInfo[playerNum].push_back((*state->getPlayer(playerNum)));

	bool allSlow = true;
	for (int i = 0; i < pastInfo[playerNum].size(); i++) {
		if (pastInfo[playerNum][i].getFSpeed() > 0.25f || pastInfo[playerNum][i].getFSpeed() < -0.25f) {
			allSlow = false;
		}
	}

	if (collisionRecovery) {
		collisionRecoveryCounter++;
		if (collisionRecoveryCounter > collisionRecoveryCounterMax) {
			collisionRecoveryCounter = 0;
			if (!allSlow) {
				collisionRecovery = false;
			}
		}
	}

	if (allSlow && !collisionRecovery) {
		collisionStartTime = timer.getCurrentTime();
		collisionRecovery = true;
		infoAtCollision = *state->getPlayer(playerNum);
	}
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

	updateRecovery(playerNum);

	/*
	// Keeps a sort of cyclical vector
	if (pastInfo[playerNum].size() < 20) {
		pastInfo[playerNum].erase(pastInfo[playerNum].begin());
	}
	pastInfo[playerNum].push_back((*state->getPlayer(playerNum)));

	bool allSlow = true;
	for (int i = 0; i < pastInfo[playerNum].size(); i++) {
		if (pastInfo[playerNum][i].getFSpeed() > 1.0f || pastInfo[playerNum][i].getFSpeed() < 1.0f) {
			allSlow = false;
		}
	}

	if (collisionRecovery) {
		collisionRecoveryCounter++;
		if (collisionRecoveryCounter > collisionRecoveryCounterMax) {
			collisionRecovery = false;
			collisionRecoveryCounter = 0;
		}
	}

	if (allSlow) {
		collisionRecovery = true;
		infoAtCollision = *state->getPlayer(playerNum);
		return recover(playerNum);
	}*/

	if (collisionRecovery)
		return recover(playerNum);

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

int AIManager::indexOfClosestPowerup(int playerID)
{
	vec3 playerPos = state->getPlayer(playerID)->getPos();
	float currentIndex = -1;
	vec3 positionOfClosest = vec3(1000, 1000, 1000);	//Huge number... should be good for our level
	for (int i = 0; i < state->numberOfPowerupBoxes(); i++)
	{
		PowerupBox* p = state->getPowerupBox(i);
		vec3 powerupPos = p->getPos();
		if (!p->getCollided())
		{
			if (abs(length(playerPos - positionOfClosest)) > abs(length(playerPos - powerupPos)))
			{
				positionOfClosest = powerupPos;
				currentIndex = i;
			}
		}
	}
	return currentIndex;
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
	Entity* ai = state->getPlayer(playerNum);
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
	input.tiltBackward = 0;
	input.tiltForward = 0;
	input.spinL = 0;
	input.spinR = 0;
	input.rollL = 0;
	input.rollR = 0;

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
		if (!reversing[playerNum] && speed > 0) {
			// Do not turn as direction of turning will be changing soon
			input.turnL = 0;
			input.turnR = 0;
		}
		// If moving backwards
		else if ((!reversing[playerNum] && speed <= 0) || reversing[playerNum]) {
			reversing[playerNum] = true;
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
		if (reversing[playerNum] && speed < 0) {
			input.turnL = 0;
			input.turnR = 0;
		}
		// If moving forwards
		else if ((reversing[playerNum] && speed >= 0) || !reversing[playerNum]) {
			reversing[playerNum] = false;
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

	return input;
}

Input AIManager::driveToPoint(int playerNum, vec3 pos) {
	lastDriveToPos = pos;

	
	
	Entity* ai = state->getPlayer(playerNum);
	vec3 aiPos = ai->getPos();

	Input input = Input();
	input.forward = carSpeed;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;
	input.tiltBackward = 0;
	input.tiltForward = 0;
	input.spinL = 0;
	input.spinR = 0;
	input.rollL = 0;
	input.rollR = 0;

	

	float dot = facing(ai, pos);
	float side = beside(ai, pos);

	
	
	// If not facing towards point
	if (dot < 0.95f) {
		// Turn
		if (side < 0) {
			input.turnR = -side;
		}
		else {
			input.turnL = side;
		}
	}

	prevPosition[playerNum] = aiPos;

	//AI has a powerup
	if (state->getPlayer(playerNum)->getCurrentPowerup() != -1)
	{
		if (state->getPlayer(playerNum)->getCurrentPowerup() != POWERUPS::BOMB){
			input.powerup = true;
		}
		else {
			double dist = abs(length(state->getPlayer(playerNum)->getPos() - state->getGoldenBuggy()->getPos()));

			if (dist <= 5)
			{
				//Use the bomb powerup if the AI is close to the golden buggy
				input.powerup = true;
			}
		}
	}

	return input;
}

bool AIManager::atPoint(int playerNum, vec3 pos) {
	PlayerInfo* ai = state->getPlayer(playerNum);

	float distance = length(ai->getPos() - vec3(pos.x, ai->getPos().y, pos.z));
	if (distance < 6.f) {
		return true;
	}
	else {
		 return false;
	}
}

void AIManager::clear() {
	inputs.clear();
	pastInfo.clear();

	recoveryThreshold = 60;
	collisionRecovery = false;
	collisionRecoveryCounter = 0;
	collisionRecoveryCounterMax = 1 * 60;

	pathToGoal.clear();
	nodesToGoal.clear();
	pointOnPath.clear();
	prevPosition.clear();

	lastDot = NULL;
	lastTurnL = false;
	frameCounter = 0;
	reversing.clear();
	carSpeed = 1.0f;
	// Default drive to position
	lastDriveToPos = vec3(-13, 0.5, -15);
	indexOfGoldenBuggy = 0;
	aiType = 0;
}

#endif
