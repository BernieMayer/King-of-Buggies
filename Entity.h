#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Entity {
protected:
	vec3 forward;
	mat4 transform;
	unsigned int renderID;
	unsigned int physicsID;
	Entity();
	
public:
	void setTransform(const mat4& _tranform);
	mat4 getTransform();

	vec3 getPos();
	vec3 getForward();
	vec3 getUp();
	virtual vec3 getVelocity();

	void setPos(const vec3& _pos);

	void setRenderID(unsigned int newID);
	void setPhysicsID(unsigned int newID);

	unsigned int getRenderID();
	unsigned int getPhysicsID();
};

#endif //ENTITY_H