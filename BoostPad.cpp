
#include "BoostPad.h"

void BoostPad::setPoints(std::vector<vec3> newPoints){
	points = newPoints;
}

std::vector<vec3> BoostPad::getPoints(){
	return points;
}

void BoostPad::setDefault(vec3 newCenter)
{
	center = newCenter;

	std::vector<vec3> v;

	double defaultDim = 10;

	v.push_back(newCenter - vec3(defaultDim / 2, 0, defaultDim / 2));
	v.push_back(newCenter - vec3(defaultDim / 2, 0, -defaultDim / 2));
	v.push_back(newCenter + vec3(defaultDim / 2, 0, defaultDim / 2));
	v.push_back(newCenter + vec3(defaultDim / 2, 0, -defaultDim / 2));

	setPoints(v);

	setPos(newCenter);
}

BoostPad::BoostPad(){
}

