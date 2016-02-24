#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"

void  AIManager::initAI() {
	
}




Input* AIManager::updateAI(GameState* state) {

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
		
		return &in;

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

		return &in;
	}
}

Input AIManager::testAI(PxTransform ourTransform, PxTransform ballTransform) {
	// Counter keeps track of how many frames have gone past since target last evaluated
	if (updateCounter == 0) {
		lastOurTransform = ourTransform;
		lastBallTransform = ballTransform;
		inputs.clear();

		// Find how much rotation is needed to turn towards ball
		// Set rotation target to half this value
	}

	// If rotatation at rotation target
	// Go through input in reverse

	// If not, find out which direction to turn and make input turn that way
	// Give full forwards input
	// Store input in inputs
	// Increment counter
	// return current input

	updateCounter++;
	return Input();
}

#endif