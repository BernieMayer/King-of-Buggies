#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "PlayerInfo.h"
#include "Coin.h"
#include "Powerup.h"
#include "PowerupBox.h"
#include "GameStateLoader.h"
#include "LevelInfo.h"

const unsigned int NUM_PLAYERS = 4;
const unsigned int NUM_COINS = 10;
const unsigned int NUM_POWERUPS = 5;
const unsigned int NUM_POWERBOXES = 10;

class GameState {
private:
	LevelInfo map;
	PlayerInfo players[NUM_PLAYERS];
	Coin coins[NUM_COINS];
	Powerup powerups[NUM_POWERUPS];
	PowerupBox boxes[NUM_POWERBOXES];

public:
	GameState();

	void setPlayer(unsigned int playerNum, const PlayerInfo& pinfo);
	PlayerInfo* getPlayer(unsigned int playerNum);

	void setCoin(unsigned int coinNum, const Coin& coin);
	Coin* getCoin(unsigned int coinNum);

	void setPowerup(unsigned int powerupNum, const Powerup& powerup);
	Powerup* getPowerup(unsigned int powerupNum);

	void setPowerupBox(unsigned int boxNum, const PowerupBox& box);
	PowerupBox* getPowerupBox(unsigned int boxNum);
};

#endif // GAMESTATE_H