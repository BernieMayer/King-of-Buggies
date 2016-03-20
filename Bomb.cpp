#include "Bomb.h"


Bomb::Bomb(int pID, int rID, vec3 newPos)
{
	physicsID = pID;
	renderID = rID;
	pos = newPos;
}


void Bomb::setPos(vec3 newPos){ pos = newPos; }
vec3 Bomb::getPos(){ return pos; }
