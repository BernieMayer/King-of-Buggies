#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

// Loads the indices of meshes contained in MeshInfo
enum { CUBE = 0, SPHERE, PLANE, COUNT };

class MeshInfo {
public:
	MeshInfo();

	MeshObject getMesh(int objectType);
	vector<vec3> getMeshVertices(int objectType);
	vector<vec3> getMeshNormals(int objectType);
	vector<unsigned int> getMeshIndices(int objectType);

	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
};

#endif // MESHINFO_H