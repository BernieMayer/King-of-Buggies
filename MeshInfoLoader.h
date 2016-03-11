// File originally partially created by Troy Alderson for use in CPSC453 assignments. Reused with permission from Ben Stephenson

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
	MeshInfoLoader() {};

	bool loadModel(char* filename);
	void clearData();

	//void calculateNormals ();
	//void flipFaces();

	/*int numVertices ();
	GLv3f getVertex (int i);
	GLv3f getNormal (int i);
	void setVertex (int i, GLv3f v);
	void setNormal (int i, GLv3f v);*/
	vec3 getCenter();
	float getBoundingRadius();
	float getBottom();

	//GLuint vertexBuffer, indexBuffer;

	vector<vec3> vertices, normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;

	unsigned char LoadTexture(const char * filename);

protected:

private:

	vec3 max, min;
};

#endif // MESHINFOLOADER_H
