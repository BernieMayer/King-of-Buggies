#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"


void  AIManager::initAI(int pNum) {
	playerNum = pNum;
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
	vec = vec * vec3(6, 6, 6); // we project N frames into the future

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
			if (yaw_golden - yaw_chaseAI){
				input.turnL = 1.0;
			}
			else {
				input.turnR = 1.0;
			}

		}
		else {
			//TODO fix thus up
			input.turnL = 0.5;


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

Input AIManager::testAIEvade(GameState state) {	
	Entity* ai = state.getPlayer(playerNum);

	Entity* player = NULL;
	float greatestLength = 0;
	for (int i = 0; i < state.numberOfPlayers(); i++) {
		if (i != playerNum) {
			Entity* indexedPlayer = state.getPlayer(i);
			vec3 playerPos = indexedPlayer->getPos();
			vec3 aiPos = ai->getPos();
			vec3 distance = playerPos - aiPos;

			float len = length(distance);
			if (len > greatestLength || player == NULL) {
				greatestLength = len;
				player = indexedPlayer;
			}
		}
	}
	
	
	Input input = Input();
	input.forward = 0.5f;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;

	float dot = facing(ai, player);
	float side = beside(ai, player);

	// If not facing away from car
	// turn
	if (dot > -0.9f) {
		if (side > 0) {
			input.turnR = side;
		}
		else {
			input.turnL = -side;
		}
	}

	return input;
}

#endif