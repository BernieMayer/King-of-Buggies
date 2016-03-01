#ifndef ENTITY_CPP
#define ENTITY_CPP

#include "Entity.h"

Entity::Entity() :	transform(mat4(1.f)), renderID(-1), physicsID(-1)
{

}

void Entity::setTransform(const mat4& _transform) { transform = _transform; }
mat4 Entity::getTransform() { return transform; }

vec3 Entity::getPos() 
{
	vec4 pos = transform*vec4(0.f, 0.f, 0.f, 1.f);
	return vec3(pos)/pos.w; 
}
vec3 Entity::getForward() {
	return vec3(transform*vec4(0.f, 0.f, 1.f, 0.f)); 
}
vec3 Entity::getUp(){ 
	return vec3(transform*vec4(0.f, 1.f, 0.f, 0.f));
}

void Entity::setPos(const vec3& _pos)
{
	transform[3][0] = _pos.x;
	transform[3][1] = _pos.y;
	transform[3][2] = _pos.z;
}

void Entity::setRenderID(unsigned int newID){ 
	renderID = newID;
}

void Entity::setPhysicsID(unsigned int newID){ 
	physicsID = newID;
}

unsigned int Entity::getRenderID(){ return renderID; }
unsigned int Entity::getPhysicsID(){ return physicsID; }


#endif