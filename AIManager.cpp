#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"


void  AIManager::initAI(int pNum) {
	playerNums.push_back(pNum);
}

void  AIManager::initAI(GameState state) {
	prevPosition = state.getPlayer(0)->getPos();
}


/*




*/

Input AIManager::testAIChase(GameState state){
	/*
	point vec=hispos-prevpos; 	// vec is the 1-frame position difference
	vec=vec*N; 					// we project N frames into the future
	point futurepos=hispos+vec; // and build the future projection
	reaim(mypos,myyaw,futurepos);
	mypos.x = mypos.x + cos(myyaw) * speed;
	mypos.z = mypos.z + sin(myyaw) * speed;
	*/
	vec3 goldenBuggieLoc = state.getPlayer(0)->getPos();
	prevPosition = goldenBuggieLoc;
	vec3 myPos = state.getPlayer(1)->getPos();
	vec3 vec = goldenBuggieLoc - prevPosition;
	vec = vec * vec3(30, 30, 30); // we project N frames into the future

	vec3 futurepos = goldenBuggieLoc + vec;
	double dX_chaseAI = myPos.x;
	double dY_chaseAI = myPos.y;
	double dZ_chaseAI = myPos.z;

	double yaw_chaseAI = atan2(dZ_chaseAI, dX_chaseAI);

	double dX_golden = goldenBuggieLoc.x;
	double dY_golden = goldenBuggieLoc.y;
	double dZ_golden = goldenBuggieLoc.z;

	double yaw_golden = atan2(dZ_golden, dX_golden);


	Input input = Input();
	input.forward = 0.5f;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;

	Entity* ai = state.getPlayer(1);
	Entity* goldenBuggie = state.getPlayer(0);

	float dot = facing(ai, goldenBuggie);
	if (dot < 0.9){
		if (dot > 0){
			float result = beside(ai, goldenBuggie);

			if (result > 0){
				input.turnR = -result;
			}
			else{
				input.turnL = result;
			}

		}
		else {
			float result = beside(ai, goldenBuggie);

			if (result > 0){
				input.turnR = -result;
			}
			else{
				input.turnL = result;
			}

		}
	}

	return input;
	
	
}

Input AIManager::updateAI(GameState* state) {

	PlayerInfo* player = state->getPlayer(0);
	vec3 posPlayer = player->getPos();

	PlayerInfo* ai_state = state->getPlayer(1);
	vec3 posAI = ai_state->getPos();

	vec3 diff = posAI - posPlayer;

	if (std::abs(length(diff)) <= 7.0f ){
		Input in = Input();
		in.forward = 1;
		in.backward = 0;
		in.turnL = 0;
		in.turnR = 0;
		in.camV = 0;
		in.camH = 0;

		in.drift = false;
		in.powerup = false;
		in.menu = false;
		
		return in;
	}
	else {
		//std::cout << "AI should not move \n";
		Input in = Input();
		in.forward = 0.0;
		in.backward = 0;
		in.turnL = 0;
		in.turnR = 0;
		in.camV = 0;
		in.camH = 0;
		
		in.drift = false;
		in.powerup = false;
		in.menu = false;

		return in;
	}
}

// If result is negative, ai is facing away from player
// If result is positive ai is facing player
// Returns value between -1 and 1
float AIManager::facing(Entity* object, Entity* target) {
	vec3 targetPos = target->getPos();
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

Input AIManager::testAIEvade(GameState state, int playerNum) {	
	Entity* ai = state.getPlayer(playerNum);
	vec3 aiPos = ai->getPos();

	Entity* player = NULL;
	float shortestLength = 0;
	for (int i = 0; i < state.numberOfPlayers(); i++) {
		if (i != playerNum) {
			Entity* indexedPlayer = state.getPlayer(i);
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
	float speed = length(aiPos - prevPosition);
	

	// If not facing away from car
	if (dot > -0.9f) {
		// If facing car and very close
		if (dot >= 0.9 && distance <= 10) {
			// Go backwards
			input.forward = 0.0f;
			input.backward = carSpeed;
			// if still moving forwards
			if (!reversing && speed >= 0.003) {
				// Do not turn as direction of turning will be changing soon
				input.turnL = 0;
				input.turnR = 0;
			}
			// If moving backwards
			else if ((!reversing && speed < 0.003) || reversing) {
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
		else {
			// If still moving backwards, do not turn as turning direction will be changing soon
			if (reversing && speed >= 0.003) {
				input.turnL = 0;
				input.turnR = 0;
			}
			// If moving forwards
			else if ((reversing && speed < 0.003) || !reversing) {
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
	}

	prevPosition = aiPos;

	return input;
}

#endif
