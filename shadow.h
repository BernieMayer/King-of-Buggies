#ifndef SHADOW_H
#define SHADOW_H

#include "material.h"

class Shadow : public Material
{
public:
	Shadow();

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 viewer, vec3 light, vec3 color);

	void loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit);

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform);

	virtual unsigned int buffersUsed() { return VERTICES; }
};












#endif //SHADOW_H