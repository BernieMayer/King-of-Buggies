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
	vector<vec3> boxLocations;
	vector<vec3> startLocations;

public:
	LevelInfo();
	LevelInfo(MeshObject* mesh);
	LevelInfo(MeshObject* mesh, char *coinsInfoFile, char *boostInfoFile, char *boxInfoFile, char *startInfoFile);

	MeshObject* getMeshObject();
	vector<vec3> getCoinLocations();
	vector<vec3> getBoostLocations();
	vector<vec3> getBoxLocations();
	vec3 getStartLocation(unsigned int playerID);

};

#endif // LEVELINFO_H