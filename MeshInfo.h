#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

#include "PxPhysicsAPI.h"

// Loads the indices of meshes contained in MeshInfo
enum { BRICK = 0, CUBE, WHEEL, COIN, MINE, DONUTLEVEL, OLDLEVEL, BUGGY, UNITBUGGY, COUNT };

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
	unsigned int getGoldenBuggyTexID();

	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
	vector<unsigned int> buggyTexIDs;
	unsigned int goldenBuggyTexID;
};

#endif // MESHINFO_H