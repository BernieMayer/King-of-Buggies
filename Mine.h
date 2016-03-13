#ifndef MINE_H
#define MINE_H

#include "Powerup.h"

class Mine: public Powerup {

public:
	void setRadius(double newRaduis);
	double getRadius();

	void setPos(vec3 newPos);
	vec3 getPos();

	Mine(double newRadius, vec3 newPos);
	
private:
	double radius;
	vec3 pos;

};

#endif //MINE_H