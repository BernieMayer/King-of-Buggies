#include "InterfaceManager.h"

InterfaceManager::InterfaceManager() {}

InterfaceManager::InterfaceManager(unsigned int windowWidth, unsigned int windowHeight) : wWidth(windowWidth), wHeight(windowHeight) 
{
	barHeight = wHeight * 0.015;
}

//  this is calculated based on the player's score
float InterfaceManager::getScoreBarWidth(GameState* state) 
{
	float score = state->getGoldenBuggy()->getScore();
	
	float barRatio = score / 700.f;
	float theWidth = (wWidth * barRatio);
	
	return theWidth;
}

// this is predefined
unsigned int InterfaceManager::getScoreBarHeight() { return barHeight; }

void InterfaceManager::setWindowDim(int width, int height) 
{
	wWidth = width;
	wHeight = height;

	barHeight = wHeight * 0.02;
}
