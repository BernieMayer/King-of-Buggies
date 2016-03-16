#include "InterfaceManager.h"

InterfaceManager::InterfaceManager() {}

InterfaceManager::InterfaceManager(unsigned int windowWidth, unsigned int windowHeight) : wWidth(windowWidth), wHeight(windowHeight) 
{
	barHeight = wHeight / 100;
}

//  this is calculated based on the player's score
float InterfaceManager::getScoreBarWidth(GameState* state, unsigned int playerID) 
{
	float score = state->getPlayer(playerID)->getScore();
	
	float maxScore = state->getMaxScore();

	float barRatio = score / maxScore;
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
	vector<vector<vec3>> scorebarQuads;

	int numPlayers = state->numberOfPlayers();
	int barOffset = 0;
	
	for (int i = 0; i < numPlayers; i++) {
		float barWidth = getScoreBarWidth(state, i);
		vector<vec3> quadCoords;

		// Store the four corners of the scorebar
		quadCoords.push_back(vec3(0.f, barOffset, 0.f));
		quadCoords.push_back(vec3(barWidth, barOffset, 0.f));
		quadCoords.push_back(vec3(barWidth, (barOffset + 10), 0.f));
		quadCoords.push_back(vec3(0.f, (barOffset + 10), 0.f));
		
		scorebarQuads.push_back(quadCoords);
		barOffset += 10;
	}
	return scorebarQuads;
}