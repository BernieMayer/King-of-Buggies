#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"

void  AIManager::initAI() {
	
}

Input AIManager::updateAI(GameState* state) {

	PlayerInfo* player = state->getPlayer(0);
	vec3 posPlayer = player->getPos();

	PlayerInfo* ai_state = state->getPlayer(1);
	vec3 posAI = ai_state->getPos();

	vec3 diff = posAI - posPlayer;

	if (std::abs(diff.length()) <= 7.0f ){
		//cout << "diff.length() is " << diff.length() << "\n";
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
		std::cout << "AI should not move \n";
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
float AIManager::facingPlayer(GameState state) {
	PlayerInfo* player = state.getPlayer(0);
	PlayerInfo* ai = state.getPlayer(1);

	vec3 playerPos = player->getPos();
	vec3 aiPos = ai->getPos();
	vec3 aiForward = ai->getForward();

	vec3 difference = playerPos - aiPos;
	float result = dot(difference, aiForward);

	return result;
}

Input AIManager::testAIEvade(GameState state) {
	float dot = facingPlayer(state);
	
	Input input = Input();
	input.forward = 1.0f;
	input.backward = 0;
	input.turnL = 0;
	input.turnR = 0;


	if (lastDot == NULL) {
		lastDot = dot;

		input.turnR = 1.0f;
	}
	else {
		if (lastDot > dot) {
			input.turnR = 1;
		}
		else {
			input.turnL = 1;
		}

		lastDot = dot;
	}

	cout << input.forward << "\n";

	return input;
}

#endif