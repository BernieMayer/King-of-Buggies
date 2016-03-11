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

	enum {DIFFUSE=0, DIFFUSE_TEX, SPECULAR, SPECULAR_TEX, TORRANCE_SPARROW, TORRANCE_SPARROW_TEX, COUNT};
	
	GLuint shaderIDs[COUNT];
	void initShaders();		//Must call after glewInit but before using materials

};

extern ShaderList shaderList;



class Material
{
protected:
	GLuint programID;

	bool verticesUsed;
	bool normalsUsed;
	bool uvsUsed;
public:

	Material();

	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform, 
		vec3 viewer, vec3 light, vec3 color);
	virtual void loadUniforms(const mat4& transform, const mat4& objectTransform,
		vec3 viewer, vec3 light, unsigned int texID, unsigned int texUnit);
	void useShader();

	bool usingVertices();
	bool usingNormals();
	bool usingUvs();
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


#endif	//MATERIAL_H