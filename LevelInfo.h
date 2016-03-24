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
	vector<vec3> boostLocations;

public:
	LevelInfo();
	LevelInfo(MeshObject* mesh);
	LevelInfo(MeshObject* mesh, char *coinsInfoFile, char* boostInfoFile);

	MeshObject* getMeshObject();
	vector<vec3> getCoinLocations();
	vector<vec3> getBoostLocations();

};

#endif // LEVELINFO_H