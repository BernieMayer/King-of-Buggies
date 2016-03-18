#ifndef BOMB_H
#define BOMB_H

#include "Powerup.h"

class Bomb : public Powerup {

public:
	void setPos(vec3 newPos);
	vec3 getPos();

	Bomb(int pID, int rID, vec3 newPos);
	void setRenderID(int rID) { renderID = rID; }

private:
	vec3 pos;
};

#endif //BOMB_H