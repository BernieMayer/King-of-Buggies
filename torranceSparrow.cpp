#ifndef TORRANCESPARROW_CPP
#define TORRANCESPARROW_CPP

#include "torranceSparrow.h"

TorranceSparrow::TorranceSparrow() : shininess(3)
{
	shaderIndex = ShaderList::TORRANCE_SPARROW;
	programID = shaderList.shaderIDs[ShaderList::TORRANCE_SPARROW];
}

TorranceSparrow::TorranceSparrow(float _shininess) : shininess(_shininess)
{
	shaderIndex = ShaderList::TORRANCE_SPARROW;
	programID = shaderList.shaderIDs[ShaderList::TORRANCE_SPARROW];
}

void TorranceSparrow::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewPos, vec3 light, vec3 color)
{
	Material::loadUniforms(transform, objectTransform, viewPos, light, color);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);

	glErrorCheckMaterial("Uniforms - TorranceSparrow");
}

void TorranceSparrow::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit)
{
	Material::loadUniforms(transform, objectTransform, viewer, light, texID, texUnit);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);

	glErrorCheckMaterial("Uniforms - TorranceSparrow Texture");

}

void TorranceSparrow::loadUniforms(const mat4& transform, const mat4& objectTransform, const mat4& shadowTransform, vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit, unsigned int shadowID, unsigned int shadowTexUnit, float* randomPoints, unsigned int randomPointsNum)
{
	Material::loadUniforms(transform, objectTransform, shadowTransform, viewer, light, texID, texUnit, shadowID, shadowTexUnit, randomPoints, randomPointsNum);

	GLuint uniformLocation = glGetUniformLocation(programID, "shininess");
	glUniform1f(uniformLocation, shininess);

	glErrorCheckMaterial("Uniforms - TorranceSparrow Shadow");
}

void TorranceSparrow::setShininess(float _shininess) { shininess = _shininess; }



#endif //TORRANCESPARROW_CPP