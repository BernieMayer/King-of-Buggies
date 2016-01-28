#ifndef RENDERER_H
#define RENDERER_H

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "MeshObject.h"
#include "Material.h"

using namespace std;
using namespace glm;

enum SHADOW_BEHAVIOUR {CAST=0, RECEIVE, CAST_AND_RECEIVE, NONE};

const unsigned int DEFAULT_WIDTH = 800;
const unsigned int DEFAULT_HEIGHT = 800;


class Renderer {
private:
	class ObjectInfo{
	public:
		ObjectInfo();

		vector<vec3>* mesh;
		vector<vec3>* normals;
		vector<vec2>* uvs;
		vector<unsigned int>* indices;
		Material* mat;
		vec4 color;
		int shadowBehaviour;
		bool deleted;

		//Information about object orientation
		vec3 position;
		vec3 forward;
		vec3 up;
	};

	GLFWwindow * window;
	vector<ObjectInfo> objects;		//Objects to be drawn

	mat4 transform;		//Transformation matrix

	/**
	* Functions
	**/

	void loadBuffers(Material* mat);

public:
	Renderer(GLFWwindow* _window);
	void renderLoop();

	//Transformations
	void loadModelviewTransform(const mat4& _modelview);
	void loadProjectionTransform(const mat4& _projection);
	void loadPerspectiveTransform(float near, float far, float fov);
	void loadOrthographicTransform(float near, float far, float width, float height);

	//Drawable objects
	unsigned int generateObjectID();
	void assignMesh(unsigned int object, vector<vec3>* mesh);
	void assignNormals(unsigned int object, vector<vec3>* normals);
	void assignUVs(unsigned int object, vector<vec2>* uvs);
	void assignIndices(unsigned int object, vector<unsigned int>* indices);		//If no indices are assigned, will traverse mesh in order
	void assignColor(unsigned int object, const vec4& color);
	void setShadowBehaviour(int object, int behaviour);
	void assignMaterial(unsigned int object, Material* mat);		

	//Lights
	unsigned int generateLightObject();
	void assignLightMesh(unsigned int object, vector<vec3>* mesh);
	void assignLightColor(unsigned int object, const vec4& color);
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