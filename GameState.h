#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "PlayerInfo.h"
#include "GameStateLoader.h"
#include "LevelInfo.h"

class GameState {
private:
	LevelInfo map;
	PlayerInfo players[4];

public:
	GameState();


};

#endif // GAMESTATE_H