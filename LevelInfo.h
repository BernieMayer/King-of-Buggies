#ifndef LEVELINFO_H
#define LEVELINFO_H

#include <glm/glm.hpp>

#include "LevelInfoLoader.h"
#include "MeshObject.h"

using namespace glm;

class LevelInfo {
private:
	LevelInfoLoader loader;
	MeshObject* arena;
	vector<vec3> coinLocations;

public:
	LevelInfo();
	LevelInfo(MeshObject* mesh);
	LevelInfo(MeshObject* mesh, char *coinsInfoFile);

	MeshObject* getMeshObject();
	vector<vec3> getCoinLocations();

};

#endif // LEVELINFO_H