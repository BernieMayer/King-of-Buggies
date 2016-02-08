#ifndef ENTITY_CPP
#define ENTITY_CPP

#include "Entity.h"

Entity::Entity() :	forward(vec3(0.f, 0.f, 1.f)), up(vec3(0.f, 1.f, 0.f)),
pos(vec3(0.0, 0.0, 0.0)), transform(mat4(1.f)), renderID(-1), physicsID(-1)
{

}

void Entity::setTransform(const mat4& _transform) { transform = _transform; }
mat4 Entity::getTransform() { return transform; }

vec3 Entity::getPos() { return pos; }
vec3 Entity::getForward() { return forward; }
vec3 Entity::getUp(){ return up; }

int Entity::getRenderID(){ return renderID; }
int Entity::getPhysicsID(){ return physicsID; }


#endif