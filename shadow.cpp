#ifndef SHADOW_CPP
#define SHADOW_CPP

#include "shadow.h"

Shadow::Shadow()
{
	shaderIndex = ShaderList::SHADOW;
	programID = shaderList.shaderIDs[ShaderList::DIFFUSE];
}

void Shadow::loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 viewer, vec3 light, vec3 color)
{

}

void Shadow::loadUniforms(const mat4& transform, const mat4& objectTransform)
{
	GLuint uniformLocation = glGetUniformLocation(programID, "transform");
	glUniformMatrix4fv(uniformLocation, 1, false, &transform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "objectTransform");
	glUniformMatrix4fv(uniformLocation, 1, false, &objectTransform[0][0]);

	glErrorCheckMaterial("Uniforms");
}









#endif //SHADOW_CPP