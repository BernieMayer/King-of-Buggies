#ifndef GAMESTATE_CPP
#define GAMESTATE_CPP

#include "GameState.h"

GameState::GameState()
{

}

void GameState::setPlayer(unsigned int playerNum, const PlayerInfo& pinfo)
{
	if (playerNum < NUM_PLAYERS)
		players[playerNum] = pinfo;
	else
		printf("PlayerInfo>> Player index out of range\n");
}

PlayerInfo* GameState::getPlayer(unsigned int playerNum)
{
	if (playerNum < NUM_PLAYERS)
		return &players[playerNum];
	else
		return NULL;
}




#endif // GAMESTATE_CPP