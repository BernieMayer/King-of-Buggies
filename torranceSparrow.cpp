#ifndef TORRANCESPARROW_CPP
#define TORRANCESPARROW_CPP

#include "torranceSparrow.h"

TorranceSparrow::TorranceSparrow() : shininess(3)
{
	verticesUsed = true;
	normalsUsed = true;
	uvsUsed = false;

	programID = shaderList.shaderIDs[ShaderList::TORRANCE_SPARROW];
}

TorranceSparrow::TorranceSparrow(float _shininess) : shininess(_shininess)
{
	verticesUsed = true;
	normalsUsed = true;
	uvsUsed = false;

	programID = shaderList.shaderIDs[ShaderList::TORRANCE_SPARROW];
}

void TorranceSparrow::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewPos, vec3 light, vec3 color)
{
	Material::loadUniforms(transform, objectTransform, viewPos, light, color);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);
}

void TorranceSparrow::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit)
{
	Material::loadUniforms(transform, objectTransform, viewer, light, texID, texUnit);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);

}

void TorranceSparrow::setShininess(float _shininess) { shininess = _shininess; }



#endif //TORRANCESPARROW_CPP