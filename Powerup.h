#ifndef POWERUP_H
#define POWERUP_H

#include "Entity.h"

/*
0 - Nitro Boost
1 - Bomb
2 - Mine
3 - Coin?
4 - Decoy?
*/
struct POWERUPS{
	enum { NITROBOOST = 0, MINE, BOMB, COIN, DECOY, COUNT };
};

class Powerup : public Entity {
protected:
	int type;
	vec3 velocity;

public:
	Powerup() {};

	void setTransform(const mat4& transform, float time);
	vec3 getVelocity();
};

#endif //POWERUP_H