#ifndef LEVELINFO_H
#define LEVELINFO_H

#include <glm/glm.hpp>

#include "LevelInfoLoader.h"

using namespace glm;

class LevelInfo {
private:
	float** arena;
	vec3 powerUpLocations;
};

#endif // LEVELINFO_H