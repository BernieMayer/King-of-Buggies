#include <glm/glm.hpp>
#include <vector>

#include "MeshObject.h"

using namespace std;
using namespace glm;

MeshObject::MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<vec2> initUvs, vector<unsigned int> initIndices) {
	vertices = initVertices;
	normals = initNormals;
	uvs = initUvs;
	indices = initIndices;
}

vector<vec3> MeshObject::getVertices() {
	return vertices;
}

vector<vec3> MeshObject::getNormals() {
	return normals;
}

vector<vec2> MeshObject::getUvs() {
	return uvs;
}

vector<unsigned int> MeshObject::getIndices() {
	return indices;
}

vector<vec3>* MeshObject::getVertexPointer()
{
	return &vertices;
}
vector<vec3>* MeshObject::getNormalPointer()
{
	return &normals;
}
vector<vec2>* MeshObject::getUvPointer()
{
	return &uvs;
}
vector<unsigned int>* MeshObject::getIndexPointer()
{
	return &indices;
}
