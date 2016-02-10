#ifndef LEVELINFO_H
#define LEVELINFO_H

#include <glm/glm.hpp>

#include "LevelInfoLoader.h"
#include "MeshObject.h"

using namespace glm;

class LevelInfo {
private:
	MeshObject* arena;
	vec3 powerUpLocations;
public:
	LevelInfo();
	LevelInfo(MeshObject* mesh);

	MeshObject* getMeshObject();

};

#endif // LEVELINFO_H