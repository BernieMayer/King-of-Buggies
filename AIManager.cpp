#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"

void  AIManager::initAI() {
	
}


/*




*/

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

Input AIManager::testAIEvade(GameState state) {
	Entity* ai = state.getPlayer(1);
	Entity* player;
	if (frameCounter == 0) {
		 player = state.getPlayer(0);
		 lastState = state;
	}
	else {
		player = lastState.getPlayer(0);
	}
	
	float dot = facing(ai, player);

	cout << "Dot: " << dot << "\n";
	
	Input input = Input();
	input.forward = 0.5f;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;


	if (lastDot != NULL) {
		// If not facing away from car
		// turn
		if (dot > -0.9f) {
			// If last input turned towards car and was left
			// turn right
			if (lastDot < dot && lastTurnL) {
				input.turnR = 1;
				lastTurnL = false;
			}
			// If last input turned towards car and was right
			// turn left
			else if (lastDot < dot && !lastTurnL) {
				input.turnL = 1;
				lastTurnL = true;
			}
			else {
				// If last input turned away from car and was left
				// turn left
				if (lastTurnL) {
					input.turnL = 1;
				}
				// If last input turned away from car and was right
				// turn right
				else {
					input.turnR = 1;
				}
			}
		}
		else {
			lastTurnL = false;
		}
	}

	lastDot = dot;

	if (frameCounter != 20) {
		frameCounter++;
	}
	else {
		frameCounter = 0;
	}
	
	return input;
}

#endif