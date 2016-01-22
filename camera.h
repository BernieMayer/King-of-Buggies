#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdio>

const float minZoom = 0.05;
const float maxZoom = 5.0;

const int FREEROAM_CAMERA = 0;
const int MODELVIEWER_CAMERA = 1;

using namespace glm;

class Camera
{
	vec3 dir;
	vec3 up;
	vec3 right;
	vec3 pos;
	vec3 viewCenter;

	int cameraMode;

public:
	Camera();

	Camera(vec3 _dir, vec3 _up, vec3 _pos);

	Camera(vec3 _dir, vec3 _up, vec3 _pos, int _cameraMode);
	
	vec3 getPos();

	vec3 getDir();

	void changeDir(vec3 _dir);

	void changePos(vec3 _pos);

	void changeViewCenter(vec3 _viewCenter);

	mat4 getMatrix();

	void moveForward(float z);

	void moveRight(float x);

	void moveUp(float y);
	
	void move(vec3 direction);

	void moveCenter(vec3 direction);

	void rotateView(float x, float y);

	void rotateViewAround(float x, float y);

	void zoom(float scale);

};
