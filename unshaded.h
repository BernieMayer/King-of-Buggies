#ifndef UNSHADED_H
#define UNSHADED_H

#include "material.h"

class Unshaded : public Material
{
public:
	Unshaded();

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, vec3 color);

	void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit);
};



#endif	//UNSHADED_H