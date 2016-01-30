#ifndef MATERIAL_H
#define MATERIAL_H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<GL/glew.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

const string shaderDir = "./shaders/";

class Material
{
protected:
	GLuint programID;
	string shader_name;
	bool verticesUsed;
	bool normalsUsed;
	bool uvsUsed;
public:
	Material();

	void loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 light, vec3 color);
	void useShader();

	bool usingVertices();
	bool usingNormals();
	bool usingUvs();
};




/**
* Shader compilation and initialization
**/
GLuint CreateShaderProgram(const string & vsSource,
	const string & fsSource);

GLuint CreateShaderProgram(const string & vsSource,
	const string & gsSource,
	const string & fsSource);

bool checkCompileStatus(GLint shaderID);
bool checkLinkStatus(GLint programID);

string loadShaderStringFromFile(const string & filePath);


#endif	//MATERIAL_H