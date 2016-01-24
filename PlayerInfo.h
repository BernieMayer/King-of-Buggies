#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <glm/glm.hpp>
#include "Entity.h"

class PlayerInfo: public Entity {
public:
	bool isGoldenBuggie();

private:
	int powerup;
	int numCoins;
	int powerupCounter;
	float timeGolden;
};

#endif // PLAYERINFO_H