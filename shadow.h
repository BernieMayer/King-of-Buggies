#ifndef SHADOW_H
#define SHADOW_H

#include "material.h"

class Shadow : public Material
{
public:
	Shadow();

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 viewer, vec3 light, vec3 color);

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform);
};












#endif //SHADOW_H