#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <glm/glm.hpp>
#include <vector>

#include "PxPhysicsAPI.h"

using namespace std;
using namespace glm;

class MeshObject {
public:
	MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<vec2> initUvs, vector<unsigned int> initIndices);
	MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<vec2> initUvs, vector<unsigned int> initIndices, unsigned int texID);
	vector<vec3> getVertices();
	vector<vec3> getNormals();
	vector<vec2> getUvs();
	vector<unsigned int> getIndices();

	vector<vec3>* getVertexPointer();
	vector<vec3>* getNormalPointer();
	vector<vec2>* getUvPointer();
	vector<unsigned int>* getIndexPointer();

	vector<physx::PxVec3> getPhysicsVertices();
	vector<physx::PxVec3> getPhysicsIndices();
	vector<physx::PxVec3>* getPhysicsVertexPointer();
	vector<physx::PxVec3>* getPhysicsIndexPointer();

	unsigned int getTextureID();

	void createPhysicsMesh();
	void setupTextureInfo(unsigned char data, unsigned int width, unsigned int height);

	float getBoundingRadius();
	vec3 getCenter();

private:
	vector<vec3> vertices, normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;
	vector<physx::PxVec3> physicsVertices, physicsIndices;

	unsigned int textureID;
};

#endif // MESHOBJECT_H