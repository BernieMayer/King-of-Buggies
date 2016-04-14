#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>

#include "PlayerInfo.h"
#include "Coin.h"
#include "Mine.h"
#include "BoostPad.h"
#include "Powerup.h"
#include "PowerupBox.h"
#include "GameStateLoader.h"
#include "LevelInfo.h"
#include "Events.h"
#include "VehicleCollisionEvent.h"
#include "VehicleWallCollisionEvent.h"
#include "GoldenBuggySwitchEvent.h"
#include "VehicleBombCollisionEvent.h"
#include "BombCreationEvent.h"
#include "Definitions.h"

const unsigned int NUM_PLAYERS = 4;
const unsigned int NUM_POWERUPS = 5;
const unsigned int NUM_POWERBOXES = 10;
const unsigned int NUM_DECOYS = 1;

class GameState {
private:
	LevelInfo map;

	vector<PlayerInfo> ai;
	vector<PlayerInfo> players;
	vector<PlayerInfo> decoys;
	vector<Coin> coins;
	vector<Mine> mines;
	vector<BoostPad> boostPads;
	vector<Powerup> powerups;
	vector<PowerupBox> boxes;
	vector<Event*> events;


	unsigned int groundPhysicsID;
	unsigned int groundRenderID;

	unsigned int goldenBuggy;

	unsigned int maxScore;
	
	unsigned int levelID;

public:
	GameState();
	GameState(MeshObject* selectedLevel, char *coinLocations, char *boostLocations);

	//Event management
	void pushEvent(Event* newEvent);		//Pointer must be allocated on heap (using new)
	Event* getEvent(unsigned int eventNum);
	int getNbEvents() { return events.size(); }
	void clearEvents();

	//Player & ai management
	void addPlayer(const PlayerInfo& pinfo);
	void addAI(const PlayerInfo& pinfo);
	void addDecoy(const PlayerInfo& decoyInfo);
	void removeDecoy();
	void setPlayer(unsigned int playerNum, const PlayerInfo& pinfo);
	PlayerInfo* getPlayer(unsigned int playerNum);
	PlayerInfo* getAI(unsigned int aiNum);
	void setGoldenBuggy(unsigned int playerNum);		//Use instead of accessing entity directly
	unsigned int numberOfPlayers();
	unsigned int numberOfAIs();
	PlayerInfo* getGoldenBuggy();
	unsigned int getGoldenBuggyID();

	void removeLastPlayer();

	unsigned int getDecoyIndex();
	bool doesDecoyExist();

	vector<vec2> setupRadar(int playerID);
	bool isWithinPolygon(vec3 pos, vector<vec3> points);

	unsigned int getMaxScore();

	//Coin management
	void addCoin(const Coin& coin);
	void setCoin(unsigned int coinNum, const Coin& coin);
	Coin* getCoin(unsigned int coinNum);
	unsigned int numberOfCoins();
	void checkRespawns();

	//BoostPad management
	void addBoostPad(const BoostPad& boostPad);
	BoostPad* getBoostPad(unsigned int boostNum);
	//TODO setBoostPad
	unsigned int numberOfBoostPads();
	bool checkBoostPadCollision(vec3 playerPos);

	//Mine management
	void addMine(const Mine& mine);
	void removeMine(unsigned int objectID);
	Mine* getMine(unsigned int mineNum);
	//TODO setMine
	unsigned int numberOfMines();
	int checkMineCollision(vec3 playerPos);

	//Powerup management
	void addPowerup(const Powerup& powerup);
	void setPowerup(unsigned int powerupNum, const Powerup& powerup);
	Powerup* getPowerup(unsigned int powerupNum);
	void removePowerup(unsigned int powerupNUum);
	unsigned int numberOfPowerups();

	//Powerup box management
	void addPowerupBox(const PowerupBox& box);
	void setPowerupBox(unsigned int boxNum, const PowerupBox& box);
	PowerupBox* getPowerupBox(unsigned int boxNum);
	unsigned int numberOfPowerupBoxes();

	// Map management
	void setMap(MeshObject* arena, unsigned int selectedLevel);
	void applyRotations();
	vec3 getStartCoord(unsigned int playerID);
	unsigned int getLevelID() { return levelID; }

	bool checkCoinCollision(vec3 playerPos);
	unsigned int numOfDecoys = 0;

	void clear();
};

#endif // GAMESTATE_H