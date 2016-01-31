#ifndef SPECULAR_CPP
#define SPECULAR_CPP

#include "specular.h"

Specular::Specular() : shininess(3)
{
	verticesUsed = true;
	normalsUsed = true;
	uvsUsed = false;

	programID = shaderList.shaderIDs[ShaderList::SPECULAR];
}

Specular::Specular(float _shininess) : shininess(_shininess)
{
	verticesUsed = true;
	normalsUsed = true;
	uvsUsed = false;

	programID = shaderList.shaderIDs[ShaderList::SPECULAR];
}

void Specular::loadUniforms(const mat4& transform, const mat4& objectTransform, 
							vec3 viewPos, vec3 light, vec3 color)
{
	Material::loadUniforms(transform, objectTransform, viewPos, light, color);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);
}

void Specular::setShininess(float _shininess) { shininess = _shininess; }



#endif //MATERIAL_CPP