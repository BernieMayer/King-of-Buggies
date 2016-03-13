
#include "Mine.h"


Mine::Mine(double newRadius, vec3 newPos)
{
	radius = newRadius;
	pos = newPos;
}


void Mine::setRadius(double newRadius)
{	
	radius = newRadius;
}

double Mine::getRadius(){ return radius; }

void Mine::setPos(vec3 newPos){ pos = newPos; }
vec3 Mine::getPos(){ return pos; }



