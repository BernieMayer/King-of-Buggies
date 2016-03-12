#ifndef SPECULAR_CPP
#define SPECULAR_CPP

#include "specular.h"

Specular::Specular() : shininess(3)
{
	shaderIndex = ShaderList::SPECULAR;
	programID = shaderList.shaderIDs[ShaderList::SPECULAR];
}

Specular::Specular(float _shininess) : shininess(_shininess)
{
	shaderIndex = ShaderList::SPECULAR;
	programID = shaderList.shaderIDs[ShaderList::SPECULAR];
}

void Specular::loadUniforms(const mat4& transform, const mat4& objectTransform, 
							vec3 viewPos, vec3 light, vec3 color)
{
	Material::loadUniforms(transform, objectTransform, viewPos, light, color);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);
}

void Specular::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit)
{
	Material::loadUniforms(transform, objectTransform, viewer, light, texID, texUnit);
	
	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);

}

void Specular::setShininess(float _shininess) { shininess = _shininess; }



#endif //MATERIAL_CPP