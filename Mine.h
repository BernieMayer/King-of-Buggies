#ifndef MINE_H
#define MINE_H

#include "Powerup.h"

class Mine: public Powerup {

public:
	void setRadius(double newRadius);
	double getRadius();

	Mine(double newRadius);
	
private:
	double radius;

};

#endif //MINE_H