#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Entity {
protected:
	vec3 forward;
	vec3 up;
	vec3 pos;
	mat4 transform;
	int renderID;
	int physicsID;
	Entity();
public:
	void setTransform(const mat4& _tranform);
	mat4 getTransform();

	vec3 getPos();
	vec3 getForward();
	vec3 getUp();

	void setRenderID();
	void setPhysicsID();

	int getRenderID();
	int getPhysicsID();
};

#endif //ENTITY_H