#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "PlayerInfo.h"
#include "GameStateLoader.h"
#include "LevelInfo.h"

const unsigned int NUM_PLAYERS = 4;

class GameState {
private:
	LevelInfo map;
	PlayerInfo players[NUM_PLAYERS];

public:
	GameState();

	void setPlayer(unsigned int playerNum, const PlayerInfo& pinfo);
	PlayerInfo* getPlayer(unsigned int playerNum);


};

#endif // GAMESTATE_H