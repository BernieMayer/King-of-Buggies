#ifndef LEVELINFO_CPP
#define LEVELINFO_CPP

#include "levelInfo.h"

LevelInfo::LevelInfo() :arena(NULL)
{

}

LevelInfo::LevelInfo(MeshObject* mesh) : arena(mesh)
{

}

LevelInfo::LevelInfo(MeshObject* mesh, char *coinsInfoFile, char *boostInfoFile) : arena(mesh)
{
	loader.loadLevelData(coinsInfoFile);
	coinLocations = loader.getData();
	loader.clearData();

	loader.loadLevelData(boostInfoFile);
	boostLocations = loader.getData();
	loader.clearData();
}

MeshObject* LevelInfo::getMeshObject() { return arena; }

vector<vec3> LevelInfo::getCoinLocations() { return coinLocations; }

vector<vec3> LevelInfo::getBoostLocations() { return boostLocations; }

#endif // LEVELINFO_CPP