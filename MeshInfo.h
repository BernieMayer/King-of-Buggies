#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

// Loads the indices of meshes contained in MeshInfo
enum { BRICK = 0, CUBE, WHEEL, COIN, COUNT };

class MeshInfo {
public:
	MeshInfo();

	MeshObject getMesh(int objectType);
	MeshObject* getMeshPointer(int objectType);
	vector<vec3> getMeshVertices(int objectType);
	vector<vec3> getMeshNormals(int objectType);
	vector<unsigned int> getMeshIndices(int objectType);

	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
};

#endif // MESHINFO_H