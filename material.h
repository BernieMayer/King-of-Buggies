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

//const string shaderDir = "./shaders/";

class ShaderList
{
public:

	enum {DIFFUSE=0, DIFFUSE_TEX, SPECULAR, SPECULAR_TEX, TORRANCE_SPARROW, TORRANCE_SPARROW_TEX, TORRANCE_SPARROW_SHADOW, UNSHADED, UNSHADED_TEX, SHADOW, COUNT};

	enum {TEXTURE_OFFSET=1, SHADOW_OFFSET=2};
	
	GLuint shaderIDs[COUNT];
	void initShaders();		//Must call after glewInit but before using materials

};

extern ShaderList shaderList;

class Material
{
protected:
	GLuint programID;
	unsigned int shaderIndex = 0;

public:

	enum {	VERTICES	= 1 << 0, 
			NORMALS		= 1 << 1, 
			UVS			= 1 << 2};

	Material();

	virtual unsigned int buffersUsed() { return VERTICES; }

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform, 
		vec3 viewer, vec3 light, vec3 color);
	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit);
	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform, const mat4& shadowTransform, vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit, unsigned int shadowTexUnit, float* randomPoints, unsigned int randomPointsNum);
	void useShader();
	void useTextureShader();
	void useShadowShader();
};




/**
* Shader compilation and initialization
**/
GLuint GetShader(const string& name);

GLuint CreateShaderProgram(const string & vsSource,
	const string & fsSource);

GLuint CreateShaderProgram(const string & vsSource,
	const string & gsSource,
	const string & fsSource);

bool checkCompileStatus(GLint shaderID);
bool checkLinkStatus(GLint programID);

string loadShaderStringFromFile(const string & filePath);

void glErrorCheckMaterial(const char* location);
#endif	//MATERIAL_H