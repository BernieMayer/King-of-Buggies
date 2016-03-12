#ifndef BOOSTPAD_H
#define BOOSTPAD_H

#include <vector>
#include "Entity.h"


class BoostPad : public Entity {
public:
	
	void setPoints(std::vector<vec3> newPoints);
	std::vector<vec3> getPoints();
	
	void setDefault(vec3 newCenter);
	BoostPad();
	

private:
	std::vector<vec3> points;
	vec3 center;
	

};

#endif //BOOSTPAD_H