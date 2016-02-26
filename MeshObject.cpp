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
	createPhysicsMesh();
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

vector<physx::PxVec3>* MeshObject::getPhysicsVertexPointer()
{
	return &physicsVertices;
}

void MeshObject::createPhysicsMesh() {
	for (int i = 0; i < this->vertices.size(); i++) {
		physx::PxF32 x = this->vertices[i].x;
		physx::PxF32 y = this->vertices[i].y;
		physx::PxF32 z = this->vertices[i].z;
		physx::PxVec3 newVertex = physx::PxVec3(x, y, z);
		physicsVertices.push_back(newVertex);
	}
}

vector<physx::PxVec3> MeshObject::getPhysicsVertices()
{
	return physicsVertices;
}