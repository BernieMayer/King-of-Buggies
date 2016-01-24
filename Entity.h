#ifndef ENTITY_H
#define ENTITY_H

#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

class Entity {
private:
	vec3 forward;
	vec3 up;
	int id;
};

#endif //ENTITY_H