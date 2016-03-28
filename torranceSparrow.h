#ifndef TORRANCESPARROW_H
#define TORRANCESPARROW_H

#include "material.h"

class TorranceSparrow : public Material
{
protected:
	float shininess;
public:
	TorranceSparrow();
	TorranceSparrow(float shininess);

	void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewPos, vec3 light, vec3 color);

	void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit);

	void setShininess(float _shininess);

	virtual unsigned int buffersUsed() { return VERTICES | NORMALS | UVS; }
};


#endif //TORRANCE_SPARROW_H