#ifndef AIMANAGER_CPP
#define AIMANAGER_CPP
#include "AIManager.h"

Input* AIManager::updateAI(GameState* state) {
	return NULL;
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