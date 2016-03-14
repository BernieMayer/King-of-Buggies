#ifndef UNSHADED_CPP
#define UNSHADED_CPP

#include "unshaded.h"

Unshaded::Unshaded()
{
	shaderIndex = ShaderList::UNSHADED;
	programID = shaderList.shaderIDs[ShaderList::UNSHADED];
}


void Unshaded::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewPos, vec3 light, vec3 color)
{

	GLuint uniformLocation = glGetUniformLocation(programID, "transform");
	glUniformMatrix4fv(uniformLocation, 1, false, &transform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "objectTransform");
	glUniformMatrix4fv(uniformLocation, 1, false, &objectTransform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "color");
	glUniform3f(uniformLocation, color.x, color.y, color.z);

	glErrorCheckMaterial("Uniforms");
}


void Unshaded::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit)
{
	GLuint uniformLocation = glGetUniformLocation(programID, "transform");
	glUniformMatrix4fv(uniformLocation, 1, false, &transform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "objectTransform");
	glUniformMatrix4fv(uniformLocation, 1, false, &objectTransform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "colorTexture");
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(uniformLocation, 0);

	glErrorCheckMaterial("Uniforms");

}












#endif //UNSHADED_CPP