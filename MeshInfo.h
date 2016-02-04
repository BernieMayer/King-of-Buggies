#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

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