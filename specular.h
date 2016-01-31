#ifndef SPECULAR_H
#define SPECULAR_H

#include "material.h"

class Specular : public Material
{
protected:
	float shininess;
public:
	Specular();
	Specular(float shininess);

	void loadUniforms(const mat4& transform, const mat4& objectTransform, 
				vec3 viewPos, vec3 light, vec3 color);

	void setShininess(float _shininess);
};


#endif SPECULAR_H