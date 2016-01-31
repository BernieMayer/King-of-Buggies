#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "material.h"

class Diffuse : public Material
{
public:
	Diffuse();

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, vec3 color);
};



#endif	//MATERIAL_H