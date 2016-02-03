#include "MeshInfo.h"

MeshInfo::MeshInfo() {
	meshLoader = MeshInfoLoader();

	meshLoader.loadModel("models\\brick.obj");
	MeshObject brickObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.indices);
	meshes.push_back(brickObject);

	meshLoader.clearData();
	
	meshLoader.loadModel("models\\sphere.obj");
	MeshObject sphereObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.indices);
	meshes.push_back(sphereObject);
	
	meshLoader.clearData();

	meshLoader.loadModel("models\\plane.obj");
	MeshObject planeObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.indices);
	meshes.push_back(planeObject);

	meshLoader.clearData();
}

MeshObject MeshInfo::getMesh(int objectType) {
	return meshes[objectType];
}

vector<vec3> MeshInfo::getMeshVertices(int objectType) {
	return meshes[objectType].getVertices();
}

vector<vec3> MeshInfo::getMeshNormals(int objectType) {
	return meshes[objectType].getNormals();
}

vector<unsigned int> MeshInfo::getMeshIndices(int objectType) {
	return meshes[objectType].getIndices();
}