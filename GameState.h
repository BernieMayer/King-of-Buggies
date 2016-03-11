#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "PlayerInfo.h"
#include "Coin.h"
#include "Powerup.h"
#include "PowerupBox.h"
#include "GameStateLoader.h"
#include "LevelInfo.h"
#include "Events.h"
#include "VehicleCollisionEvent.h"
#include "VehicleWallCollisionEvent.h"
#include "GoldenBuggySwitchEvent.h"

const unsigned int NUM_PLAYERS = 4;
const unsigned int NUM_COINS = 10;
const unsigned int NUM_POWERUPS = 5;
const unsigned int NUM_POWERBOXES = 10;

class GameState {
private:
	LevelInfo map;

	vector<PlayerInfo> ai;
	vector<PlayerInfo> players;
	vector<Coin> coins;
	vector<Powerup> powerups;
	vector<PowerupBox> boxes;
	vector<Event*> events;

	unsigned int groundPhysicsID;
	unsigned int groundRenderID;

	unsigned int goldenBuggy;

	unsigned int maxScore;
	
	

public:
	GameState();

	//Event management
	void pushEvent(Event* newEvent);		//Pointer must be allocated on heap (using new)
	Event* getEvent(unsigned int eventNum);
	int getNbEvents() { return events.size(); }
	void clearEvents();

	//Player management
	void addPlayer(const PlayerInfo& pinfo);
	void addAI(const PlayerInfo& pinfo);
	void setPlayer(unsigned int playerNum, const PlayerInfo& pinfo);
	PlayerInfo* getPlayer(unsigned int playerNum);
	PlayerInfo* getAI(unsigned int aiNum);
	void setGoldenBuggy(unsigned int playerNum);		//Use instead of accessing entity directly
	unsigned int numberOfPlayers();
	unsigned int numberOfAIs();
	PlayerInfo* getGoldenBuggy();
	unsigned int getGoldenBuggyID();


	vector<vec2> setupRadar(int playerID);
	bool isWithinPolygon(vec3 pos, vector<vec3> points);

	unsigned int getMaxScore();

	//Coin management
	void addCoin(const Coin& coin);
	void setCoin(unsigned int coinNum, const Coin& coin);
	Coin* getCoin(unsigned int coinNum);
	unsigned int numberOfCoins();

	//Powerup management
	void addPowerup(const Powerup& powerup);
	void setPowerup(unsigned int powerupNum, const Powerup& powerup);
	Powerup* getPowerup(unsigned int powerupNum);
	unsigned int numberOfPowerups();

	//Powerup box management
	void addPowerupBox(const PowerupBox& box);
	void setPowerupBox(unsigned int boxNum, const PowerupBox& box);
	PowerupBox* getPowerupBox(unsigned int boxNum);
	unsigned int numberOfPowerupBoxes();

	bool checkCoinCollision(vec3 playerPos);
};

#endif // GAMESTATE_H