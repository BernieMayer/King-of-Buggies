#include "MeshObject.h"
#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;


MeshObject::MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<unsigned int> initIndices) {
	this->vertices = initVertices;
	this->normals = initNormals;
	this->indices = initIndices;
}

vector<vec3> MeshObject::getVertices() {
	return this->vertices;
}

vector<vec3> MeshObject::getNormals() {
	return this->normals;
}

vector<unsigned int> MeshObject::getIndices() {
	return this->indices;
}

