#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <vector>
#include "MeshObject.h"
#include "Material.h"

using namespace std;
using namespace glm;

enum SHADOW_BEHAVIOUR {CAST=0, RECEIVE, CAST_AND_RECEIVE};

class Renderer {
public:
	//Drawable objects
	unsigned int generateDrawableObject();
	void assignMesh(unsigned int object, vector<vec3>* mesh);
	void assignNormals(unsigned int object, vector<vec3>* normals);
	void assignUVs(unsigned int object, vector<vec2>* uvs);
	void assignIndices(unsigned int object, vector<unsigned int>* indices);		//If no indices are assigned, will traverse mesh in order
	void setShadowBehaviour(int object, int behaviour);
	void assignMaterial(unsigned int object, Material mat);		//Make material class

	//Lights
	unsigned int generateLightObject();
	void assignLightMesh(unsigned int object, vector<vec3>* mesh);
	void assignLightColor(unsigned int object, vec4 color);
	void assignLightIntensity(unsigned int object, float intensity);
	void enableShadowCasting(unsigned int object);
	void disableShadowCasting(unsigned int object);

	//Drawing calls
	void clearBuffers();
	void draw(unsigned int object);
	void draw(vector<unsigned int> objects);	//Preferred over individual calls, easier to optimize
	void drawAll();		//Easiest to optimize

	//Delete objects
	void deleteDrawableObject(int object);
	void deleteLightObject(int object);

};

#endif // RENDERER_H