#ifndef GAMESTATE_CPP
#define GAMESTATE_CPP

#include "GameState.h"

GameState::GameState()
{
	maxScore = 2000;
}
/*
GameState::GameState(MeshObject* selectedLevel, char *coinsFile, char *boostFile)
{
	maxScore = 2000;
	map = LevelInfo(selectedLevel, coinsFile, boostFile);
	vector<vec3> coinCoords = map.getCoinLocations();
	for (unsigned int i = 0; i < coinCoords.size(); i++) {
		Coin newCoin = Coin();
		newCoin.setPos(coinCoords[i]);
		addCoin(newCoin);
	}
	vector<vec3> boostCoords = map.getBoostLocations();
	for (unsigned int i = 0; i < boostCoords.size(); i++) {
		BoostPad newBoost = BoostPad();
		newBoost.setDefault(boostCoords[i]);
		addBoostPad(newBoost);
	}
}
*/
void GameState::addPlayer(const PlayerInfo& pinfo) { players.push_back(pinfo); }
void GameState::addAI(const PlayerInfo& pinfo){ ai.push_back(pinfo); }

void GameState::addDecoy(const PlayerInfo& decoyInfo)
{
	numOfDecoys++;
	if (numOfDecoys >= NUM_DECOYS)
	{
		numOfDecoys = NUM_DECOYS;
	}
	else {
		decoys.push_back(decoyInfo);
	}
}
void GameState::removeDecoy()
{
	numOfDecoys--;
	if (numOfDecoys <= 0)
	{
		numOfDecoys = 0;
	}
	else
	{
		decoys.erase(decoys.begin());
	}
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
	if (playerNum < players.size())
		return &players[playerNum];
	else
		return NULL;
}

PlayerInfo* GameState::getAI(unsigned int aiNum)
{
	if (aiNum < ai.size())
		return &ai[aiNum];
	else
		return NULL;
}

void GameState::setGoldenBuggy(unsigned int playerNum)
{
	for (unsigned int i = 0; i < numberOfPlayers(); i++)
	{
		getPlayer(i)->setGoldenBuggy(false);
	}
	getPlayer(playerNum)->setGoldenBuggy(true);
	goldenBuggy = playerNum;
}

PlayerInfo* GameState::getGoldenBuggy()
{
	if (goldenBuggy >= players.size())
		return NULL;
	else
		return &players[goldenBuggy];
}

unsigned int GameState::getGoldenBuggyID() { return goldenBuggy; }

vector<vec2> GameState::setupRadarSeeingOnlyGoldenBuggy(int playerId)
{

	vector<vec2> vectors;

	PlayerInfo* player = getPlayer(playerId);

	vec3 player3Dpos = player->getPos();

	vec3 rightVector = player->getRight();

	vec3 player3DposUp = player->getForward();

	vec3 player3DposRight = -rightVector;
	vec3 player3DposLeft = rightVector;

	vec2 playerLeft = vec2(player3DposRight.x, -player3DposRight.z);
	vec2 playerRight = vec2(player3DposLeft.x, -player3DposLeft.z);
	vec2 playerUp = vec2(player3DposUp.x, -player3DposUp.z);


	vec3 origin = vec3((playerLeft.x + playerRight.x) / 2, 0, (playerLeft.y + playerRight.y) / 2);

	vectors.push_back(playerLeft);
	vectors.push_back(playerUp);
	vectors.push_back(playerRight);
	int radarSize = 200;
	if (playerId != getGoldenBuggyID())
	{
		vector<vec3> radarPoly;
		vec3 otherPlayerPos = getPlayer(getGoldenBuggyID())->getPos();
		radarPoly.push_back(player3Dpos - vec3(radarSize / 2, 0, radarSize / 2));
		radarPoly.push_back(player3Dpos - vec3(radarSize / 2, 0, -radarSize / 2));
		radarPoly.push_back(player3Dpos + vec3(radarSize / 2, 0, radarSize / 2));
		radarPoly.push_back(player3Dpos + vec3(radarSize / 2, 0, -radarSize / 2));


		if (isWithinPolygon(otherPlayerPos, radarPoly)){

			double dist = length((otherPlayerPos - player3Dpos));

			vec3 otherPlayerCenter = normalize(otherPlayerPos - player3Dpos);

			vec3 otherPlayer3DUp = getPlayer(getGoldenBuggyID())->getForward();
			vec3 otherPlayer3DRight = cross(getPlayer(getGoldenBuggyID())->getForward(), getPlayer(getGoldenBuggyID())->getUp());

			vec2 otherPlayerLeft = vec2(otherPlayerCenter.x - otherPlayer3DRight.x, -(otherPlayerCenter.z - otherPlayer3DRight.z));
			vec2 otherPlayerRight = vec2(otherPlayerCenter.x + otherPlayer3DRight.x, -(otherPlayerCenter.z + otherPlayer3DRight.z));
			vec2 otherPlayerUp = vec2(otherPlayerCenter.x + otherPlayer3DUp.x, -(otherPlayerCenter.z + otherPlayer3DUp.z));

			vectors.push_back(otherPlayerLeft);
			vectors.push_back(otherPlayerUp);
			vectors.push_back(otherPlayerRight);

		}
	}
	return vectors;

}

vector<vec2> GameState::setupRadar(int playerId)
{

	
	vector<vec2> vectors;

	PlayerInfo* player = getPlayer(playerId);

	vec3 player3Dpos = player->getPos();

	vec3 rightVector = player->getRight();
	
	vec3 player3DposUp = player->getForward();

	vec3 player3DposRight = -rightVector;
	vec3 player3DposLeft = rightVector;

	vec2 playerLeft =  vec2(player3DposRight.x, -player3DposRight.z);
	vec2 playerRight = vec2(player3DposLeft.x, -player3DposLeft.z);
	vec2 playerUp =  vec2(player3DposUp.x, -player3DposUp.z);


	vec3 origin = vec3((playerLeft.x + playerRight.x) / 2, 0, (playerLeft.y + playerRight.y) / 2);

	vectors.push_back( playerLeft);
	vectors.push_back( playerUp);
	vectors.push_back( playerRight);

	int radarSize = 50;
	for (unsigned int i = 0; i < numberOfPlayers(); i++)
	{
		if (i != playerId)
		{
			vector<vec3> radarPoly;
			vec3 otherPlayerPos = getPlayer(i)->getPos();
			radarPoly.push_back(player3Dpos - vec3(radarSize / 2, 0, radarSize/2));
			radarPoly.push_back(player3Dpos - vec3(radarSize / 2, 0, -radarSize / 2));
			radarPoly.push_back(player3Dpos + vec3(radarSize / 2, 0, radarSize / 2));
			radarPoly.push_back(player3Dpos + vec3(radarSize / 2, 0, -radarSize / 2));

			
			if (isWithinPolygon(otherPlayerPos, radarPoly)){
				
				double dist = length((otherPlayerPos - player3Dpos));
				
				vec3 otherPlayerCenter = normalize(otherPlayerPos - player3Dpos);

				vec3 otherPlayer3DUp =   getPlayer(i)->getForward();
				vec3 otherPlayer3DRight =cross(getPlayer(i)->getForward(), getPlayer(i)->getUp());
				
				vec2 otherPlayerLeft = vec2(otherPlayerCenter.x - otherPlayer3DRight.x, -(otherPlayerCenter.z - otherPlayer3DRight.z));
				vec2 otherPlayerRight = vec2(otherPlayerCenter.x + otherPlayer3DRight.x, -(otherPlayerCenter.z + otherPlayer3DRight.z));
				vec2 otherPlayerUp = vec2(otherPlayerCenter.x + otherPlayer3DUp.x, -(otherPlayerCenter.z + otherPlayer3DUp.z));
			
				vectors.push_back(otherPlayerLeft);
				vectors.push_back(otherPlayerUp);
				vectors.push_back(otherPlayerRight);

			}


		}
	}
	
	return vectors;
}

bool GameState::isWithinPolygon(vec3 pos, vector<vec3> points)
{
	float sign = 0.f;

	vector<vec3>::iterator it = points.begin();
	vector<vec3>::iterator end = points.end() - 1;

	vec3 segment = *it - *end;
	vec2 perp = vec2(-segment.z, segment.x);
	vec2 toPos = vec2(pos.x - (*end).x, pos.z - (*end).z);

	sign = (dot(perp, toPos) > 0) ? 1.f : -1.f;


	for (it; it + 1 != points.end(); it++)
	{
		segment = *(it + 1) - *it;
		perp = vec2(-segment.z, segment.x);
		toPos = vec2(pos.x - (*it).x, pos.z - (*it).z);

		float dot_val = dot(perp, toPos);
		//If point is on differnt side of this segment than the first, return false
		if ((dot_val > 0.f && sign <= 0.f) || (dot_val <= 0.f && sign > 0.f))
			return false;
	}

	return true;
}

unsigned int GameState::getMaxScore() { return maxScore; }

unsigned int GameState::numberOfPlayers(){ return players.size(); }
unsigned int GameState::numberOfAIs() { return ai.size(); }

void GameState::addCoin(const Coin& coin) { coins.push_back(coin); }

void GameState::setCoin(unsigned int coinNum, const Coin& coin)
{
	if (coinNum < coins.size())
		coins[coinNum] = coin;
	else
		printf("Coin>> Coin index out of range\n");
}

Coin* GameState::getCoin(unsigned int coinNum)
{
	if (coinNum < coins.size())
		return &coins[coinNum];
	else
		return NULL;
}

Mine* GameState::getMine(unsigned int mineNum)
{
	if (mineNum < mines.size())
		return &mines[mineNum];
	else
		return NULL;
}

unsigned int GameState::numberOfCoins(){ return coins.size(); }

void GameState::checkCoinRespawns() 
{
	for (unsigned int i = 0; i < coins.size(); i++) {
		vec3 pos = coins[i].getPos();

		if (coins[i].isCollided()) {
			// countdown to coin respawn
			coins[i].decrementCountdown();
			if (coins[i].getCountdown() < 0) {
				// reset coin to original spawn location
				vec3 newPos = pos;
				newPos.y = pos.y + 20;
				coins[i].setPos(newPos);
				coins[i].setCollided(false);
			}
		}
	}
}

void GameState::addBoostPad(const BoostPad& boostPad) { boostPads.push_back(boostPad); }

BoostPad* GameState::getBoostPad(unsigned int boostNum) { return &boostPads[boostNum]; }

unsigned int GameState::numberOfBoostPads(){ return boostPads.size(); }

bool GameState::checkBoostPadCollision(vec3 playerPos)
{
	bool collided = false;
	for (unsigned int i = 0; i < boostPads.size(); i++) {
		vector<vec3> points = boostPads[i].getPoints();

		if (isWithinPolygon(playerPos, points))
		{
			return true;
		}
	}
	return collided;
}

void GameState::addMine(const Mine& mine){ mines.push_back(mine); }

void GameState::removeMine(unsigned int objectID) { mines.erase(mines.begin() + objectID); }

unsigned int GameState::numberOfMines(){ return mines.size(); }

int GameState::checkMineCollision(vec3 playerPos)
{
	//bool collided = false;

	for (unsigned int i = 0; i < mines.size(); i++)
	{
		Mine mine = mines[i];
		vec3 minePos = mine.getPos();

		vec3 diff = playerPos - minePos;


		//Check if the player is within the radius of the mine
		if (abs(length(diff)) <= mine.getRadius())
		{
			//mines.erase(mines.begin() + i);
			return i;
		}
	}
	return -1;
}




void GameState::addPowerup(const Powerup& powerup) { powerups.push_back(powerup); }

void GameState::setPowerup(unsigned int powerupNum, const Powerup& powerup)
{
	if (powerupNum < powerups.size())
		powerups[powerupNum] = powerup;
	else
		printf("Powerup>> Powerup index out of range\n");
}

Powerup* GameState::getPowerup(unsigned int powerupNum)
{
	if (powerupNum < powerups.size())
		return &powerups[powerupNum];
	else
		return NULL;
}

void GameState::removePowerup(unsigned int powerupNum) {
	if (powerupNum < powerups.size()) {
		powerups.erase(powerups.begin() + powerupNum);
	}
}

unsigned int GameState::numberOfPowerups(){ return powerups.size(); }

void GameState::addPowerupBox(const PowerupBox& box) { boxes.push_back(box); }

void GameState::setPowerupBox(unsigned int boxNum, const PowerupBox& box)
{
	if (boxNum < boxes.size())
		boxes[boxNum] = box;
	else
		printf("PowerupBox>> PowerupBox index out of range\n");
}

PowerupBox* GameState::getPowerupBox(unsigned int boxNum)
{
	if (boxNum < boxes.size())
		return &boxes[boxNum];
	else
		return NULL;
}

unsigned int GameState::numberOfPowerupBoxes(){ return boxes.size(); }

bool GameState::checkCoinCollision(vec3 playerPos) {
	bool collided = false;
	for (unsigned int i = 0; i < coins.size(); i++) {
		vec3 pos = coins[i].getPos();
		
		if (!coins[i].isCollided()) {
			vec3 difference = pos - playerPos;

			//change coin to spawn below level if it is hit by a player
			if ((abs(difference.x) < 1.4) && (abs(difference.y) < 1.4) && (abs(difference.z) < 1.4)) {
				vec3 newPos = pos;
				newPos.y = pos.y - 20;
				coins[i].setPos(newPos);
				coins[i].setCollided(true);
				coins[i].startCountdown();

				collided = true;
			}
		}
	}
	return collided;
}

///////////////
// Events
//////////////

void GameState::pushEvent(Event* newEvent) 
{ 
	events.push_back(newEvent); 
}

Event* GameState::getEvent(unsigned int eventNum)
{
	return events[eventNum];
}

void GameState::clearEvents()
{
	for (unsigned int i = 0; i < events.size(); i++)
	{
		delete events[i];
	}
	events.clear();
}

void GameState::setMap(MeshObject* arena, char *coinLocations, char *boostLocations, char *boxLocations) 
{
	map = LevelInfo(arena, coinLocations, boostLocations, boxLocations);
	vector<vec3> coinCoords = map.getCoinLocations();
	vector<vec3> boostCoords = map.getBoostLocations();
	vector<vec3> boxCoords = map.getBoxLocations();

	for (unsigned int i = 0; i < coinCoords.size(); i++) {
		Coin newCoin = Coin();
		newCoin.setPos(coinCoords[i]);
		addCoin(newCoin);
	}
	for (unsigned int i = 0; i < boostCoords.size(); i++) {
		BoostPad newBoost = BoostPad();
		newBoost.setDefault(boostCoords[i]);
		addBoostPad(newBoost);
	}

	for (unsigned int i = 0; i < boxCoords.size(); i++) {
		PowerupBox newBox = PowerupBox();
		newBox.setPos(boxCoords[i]);
		addPowerupBox(newBox);
	}
}

void GameState::applyRotations()
{
	for (unsigned int i = 0; i < coins.size(); i++) {
		coins[i].setTransform(coins[i].getRotation((1.f / 60.f)*2.f*3.1415926f));
	}
	for (unsigned int i = 0; i < boxes.size(); i++) {
		boxes[i].setTransform(boxes[i].getRotation((1.f / 60.f)*2.f*3.1415926f));
	}
}

#endif // GAMESTATE_CPP