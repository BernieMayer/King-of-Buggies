#ifndef DIFFUSE_CPP
#define DIFFUSE_CPP

#include "diffuse.h"

void glErrorCheckMat(const char* location)
{
	GLenum code = glGetError();

	switch (code)
	{
	case GL_INVALID_ENUM:
		cout << "GL_INVALID_ENUM - " << location << endl;
		break;
	case GL_INVALID_VALUE:
		cout << "GL_INVALID_VALUE - " << location << endl;
		break;
	case GL_INVALID_OPERATION:
		cout << "GL_INVALID_OPERATION - " << location << endl;
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		cout << "GL_INVALID_FRAMEBUFFER_OPERATION - " << location << endl;
		break;
	case GL_OUT_OF_MEMORY:
		cout << "GL_OUT_OF_MEMORY - " << location << endl;
		break;
	}
}

Diffuse::Diffuse()
{
	shaderIndex = ShaderList::DIFFUSE;
	programID = shaderList.shaderIDs[ShaderList::DIFFUSE];
}


void Diffuse::loadUniforms(const mat4& transform, const mat4& objectTransform,
	vec3 viewer, vec3 light, vec3 color)
{
	GLuint realProgramID = shaderList.shaderIDs[ShaderList::DIFFUSE];

	GLuint uniformLocation = glGetUniformLocation(programID, "transform");
	glUniformMatrix4fv(uniformLocation, 1, false, &transform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "objectTransform");
	glUniformMatrix4fv(uniformLocation, 1, false, &objectTransform[0][0]);

	uniformLocation = glGetUniformLocation(programID, "light");
	glUniform3f(uniformLocation, light.x, light.y, light.z);

	uniformLocation = glGetUniformLocation(programID, "color");
	glUniform3f(uniformLocation, color.x, color.y, color.z);

}



#endif	//DIFFUSE_CPP