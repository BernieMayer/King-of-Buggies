#ifndef RENDERER_H
#define RENDERER_H

#int

#include <glm/glm.hpp>
#include <vector>
#include "MeshObject.h"

using namespace std;
using namespace glm;

enum SHADOW_BEHAVIOUR {CAST=0, RECEIVE, CAST_AND_RECEIVE};

class Renderer {
public:
	unsigned int generateDrawableObject();
	void assignMesh(unsigned int object, vector<vec3>* mesh);
	void assignNormals(unsigned int object, vector<vec3>* normals);
	void assignUVs(unsigned int object, vector<vec2>* uvs);
	void assignIndices(unsigned int object, vector<unsigned int>* indices);
	//void assignMaterial(unsigned int object, Material mat);		//Make material class

	void setShadowBehaviour(int object, int behaviour);



};

#endif // RENDERER_H