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

MeshObject::MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<vec2> initUvs, vector<unsigned int> initIndices, unsigned int texID) {
	vertices = initVertices;
	normals = initNormals;
	uvs = initUvs;
	indices = initIndices;
	createPhysicsMesh();

	textureID = texID;
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
	for (unsigned int i = 0; i < this->vertices.size(); i++) {
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

unsigned int MeshObject::getTextureID() { return textureID; }

float MeshObject::getBoundingRadius()
{
	float maxRadius = 0.f;

	vec3 center = getCenter();

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		maxRadius = max(length(center - vertices[i]), maxRadius);
	}

	return maxRadius;
}

vec3 MeshObject::getCenter()
{
	vec3 center (0.f, 0.f, 0.f);

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		center += vertices[i];
	}

	center /= (float)vertices.size();

	return center;
}