#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <glm/glm.hpp>
#include "Entity.h"

class PlayerInfo: public Entity {
public:
	PlayerInfo();
	bool isGoldenBuggie();

private:
	int powerup;
	int numCoins;
	int powerupCounter;
	bool goldenBuggie;
	float timeGolden;
};

#endif // PLAYERINFO_H