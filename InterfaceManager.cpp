#include "InterfaceManager.h"

InterfaceManager::InterfaceManager() {}

InterfaceManager::InterfaceManager(unsigned int windowWidth, unsigned int windowHeight) : wWidth(windowWidth), wHeight(windowHeight) 
{
	barHeight = wHeight * 0.01;
}

//  this is calculated based on the player's score
float InterfaceManager::getScoreBarWidth(GameState* state, unsigned int playerID) 
{
	float score = state->getPlayer(playerID)->getScore();
	
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

	barHeight = 10;
}

vector<vector<vec3>> InterfaceManager::generateScoreBars(GameState* state) 
{
	vector<vector<vec3>> scorebarLines;

	int numPlayers = state->numberOfPlayers();
	int barOffset = 0;
	
	for (int i = 0; i < numPlayers; i++) {
		float barWidth = getScoreBarWidth(state, i);
		vector<vec3> lineSegments;

		for (float j = 0; j < barHeight; j++) {
			lineSegments.push_back(vec3(0.f, (j + barOffset), 0.f));
			lineSegments.push_back(vec3(barWidth, (j + barOffset), 0.f));
		}
		scorebarLines.push_back(lineSegments);
		barOffset += 10;
	}
	return scorebarLines;
}