#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

class MeshObject {
public:
	MeshObject(vector<vec3> initVertices, vector<vec3> initNormals, vector<unsigned int> initIndices);
	vector<vec3> getVertices();
	vector<vec3> getNormals();
	vector<unsigned int> getIndices();

private:
	vector<vec3> vertices, normals;
	vector<unsigned int> indices;
};

#endif // MESHOBJECT_H