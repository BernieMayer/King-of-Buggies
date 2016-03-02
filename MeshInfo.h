#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

#include "PxPhysicsAPI.h"

// Loads the indices of meshes contained in MeshInfo
enum { BRICK = 0, CUBE, WHEEL, COIN, LEVEL, BUGGY, COUNT };

class MeshInfo {
public:
	MeshInfo();

	MeshObject getMesh(int objectType);
	MeshObject* getMeshPointer(int objectType);
	vector<vec3> getMeshVertices(int objectType);
	vector<vec3> getMeshNormals(int objectType);
	vector<vec2> getMeshUvs(int objectType);
	vector<unsigned int> getMeshIndices(int objectType);

	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
};

#endif // MESHINFO_H