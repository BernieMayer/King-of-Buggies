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

void GameState::setCoin(unsigned int coinNum, const Coin& coin)
{
	if (coinNum < NUM_COINS)
		coins[coinNum] = coin;
	else
		printf("Coin>> Coin index out of range\n");
}

Coin* GameState::getCoin(unsigned int coinNum)
{
	if (coinNum < NUM_COINS)
		return &coins[coinNum];
	else
		return NULL;
}

void GameState::setPowerup(unsigned int powerupNum, const Powerup& powerup)
{
	if (powerupNum < NUM_POWERUPS)
		powerups[powerupNum] = powerup;
	else
		printf("Powerup>> Powerup index out of range\n");
}

Powerup* GameState::getPowerup(unsigned int powerupNum)
{
	if (powerupNum < NUM_POWERUPS)
		return &powerups[powerupNum];
	else
		return NULL;
}

void GameState::setPowerupBox(unsigned int boxNum, const PowerupBox& box)
{
	if (boxNum < NUM_POWERBOXES)
		boxes[boxNum] = box;
	else
		printf("PowerupBox>> PowerupBox index out of range\n");
}

PowerupBox* GameState::getPowerupBox(unsigned int boxNum)
{
	if (boxNum < NUM_POWERBOXES)
		return &boxes[boxNum];
	else
		return NULL;
}

#endif // GAMESTATE_CPP