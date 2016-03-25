#include "Mine.h"


Mine::Mine(double newRadius)
{
	radius = newRadius;
	type = POWERUPS::MINE;
}


void Mine::setRadius(double newRadius)
{	
	radius = newRadius;
}

double Mine::getRadius(){ return radius; }



