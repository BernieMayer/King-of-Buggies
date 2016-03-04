#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

#include "GameState.h"

class InterfaceManager {
private:
	unsigned int wHeight;
	unsigned int wWidth;
	unsigned int barHeight;

public:
	InterfaceManager();
	InterfaceManager(unsigned int windowWidth, unsigned int windowHeight);

	float getScoreBarWidth(GameState* state, unsigned int playerID);
	unsigned int getScoreBarHeight();

	void setWindowDim(int width, int height);

	vector<vector<vec3>> generateScoreBars(GameState* state);
};

#endif // INTERFACEMANAGER_H