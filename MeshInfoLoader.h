#ifndef MESHINFOLOADER_H
#define MESHINFOLOADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;
using namespace glm;

class MeshInfoLoader {
public:
	MeshInfoLoader();
	MeshInfoLoader(char *filename);

	bool loadModel(char* filename);
	void clearData();

	//void calculateNormals ();
	//void draw (GLuint vertexLocation, GLuint normalLocation);
	//void flipFaces();

	/*int numVertices ();
	GLv3f getVertex (int i);
	GLv3f getNormal (int i);
	void setVertex (int i, GLv3f v);
	void setNormal (int i, GLv3f v);*/
	vec3 getCenter();
	float getBoundingRadius();
	float getBottom();

	/*GLuint vertexBuffer, indexBuffer;
	vector<GLfloat> vertices, normals;
	vector<GLuint> indices;*/

	vector<vec3> vertices, normals;
	vector<unsigned int> indices;
protected:

private:

	vec3 max, min;
};

#endif // MESHINFOLOADER_H

// CPSC 453: Introduction to Computer Graphics
//
// Header file for a class to handle obj file data
//
// Created by Troy Alderson
// Department of Computer Science
// University of Calgary
//
// Used with permission for CPSC 453 HW3