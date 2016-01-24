#ifndef AIMANAGER_H
#define AIMANAGER_H

#include "GameState.h"
#include "LevelInfo.h"

class AIManager {
public:
	Input* updateAI(GameState state);
};

#endif // AIMANAGER_H