#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

#include "PxPhysicsAPI.h"

// Loads the indices of meshes contained in MeshInfo
enum { BRICK = 0, CUBE, WHEEL, COIN, MINE, BOMB, BOOST, DONUTLEVEL, PIPELEVEL, OLDLEVEL, BUGGY, UNITBUGGY, COUNT };

class MeshInfo {
public:
	MeshInfo();

	MeshObject getMesh(int objectType);
	MeshObject* getMeshPointer(int objectType);
	vector<vec3> getMeshVertices(int objectType);
	vector<vec3> getMeshNormals(int objectType);
	vector<vec2> getMeshUvs(int objectType);
	vector<unsigned int> getMeshIndices(int objectType);
	unsigned int getBuggyTexID(int index);
	unsigned int getRedBuggyTexID();
	unsigned int getBlueBuggyTexID();
	unsigned int getGreenBuggyTexID();
	unsigned int getOrangeBuggyTexID();
	unsigned int getPurpleBuggyTexID();
	unsigned int getGoldenBuggyTexID();

	vector<unsigned int> buggyTexIDs;

	unsigned int getUInitroID();
	unsigned int getUIbombID();
	unsigned int getUImineID();

	unsigned int getUIcomponentID(unsigned int itemType);

	unsigned int getCoinComponentID(unsigned int number);

	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
	unsigned int redBuggyTexID;
	unsigned int blueBuggyTexID;
	unsigned int greenBuggyTexID;
	unsigned int orangeBuggyTexID;
	unsigned int purpleBuggyTexID;
	unsigned int goldenBuggyTexID;

	unsigned int UInitroID;
	unsigned int UIbombID;
	unsigned int UImineID;

	vector<unsigned int> UIcomponentIDs;

	vector<unsigned int> coinComponentIDs;
};

#endif // MESHINFO_H