// File originally partially created by Troy Alderson for use in CPSC453 assignments. Reused with permission from Ben Stephenson

#include "MeshInfoLoader.h"
#include <windows.h>
#include <fstream>
#include <soil\SOIL.h>

#pragma warning(disable:4996)

using namespace std;
using namespace glm;

void sharedIndices(vector<unsigned int> &_faces, vector<unsigned int> &_nFaces, vector<vec3> &_normals,
	vector<vec3> &vertices, vector<vec3> &normals, vector<unsigned int> &faces)
{

	normals.resize(vertices.size(), vec3(0.0, 0.0, 0.0));

	faces.clear();

	for (unsigned int i = 0; i<_faces.size(); i++)
	{
		int vi = _faces[i];
		int ni = _nFaces[i];

		if (normals[vi] == vec3())
		{
			normals[vi] = _normals[ni];
			faces.push_back(vi);
		}
		else if (normals[vi] != _normals[ni])
		{
			vertices.push_back(vertices[vi]);
			normals.push_back(_normals[ni]);
			faces.push_back(vertices.size() - 1);
		}
		else
			faces.push_back(vi);
	}
}

//Vertices not modified
void sharedIndices(vector<unsigned int> &_faces, vector<unsigned int> &_nFaces, vector<unsigned int>& _tFaces, vector<vec3>& _normals, vector<vec2>& _uvs, 
	//Returned values
	vector<vec3> &vertices, vector<vec3> &normals, vector<vec2>& uvs, vector<unsigned int> &faces)
{
	normals.resize(vertices.size(), vec3(0.0, 0.0, 0.0));

	faces.clear();
	vector<unsigned int> tempFaces;
	vector<unsigned int> tempTFaces;

	for (unsigned int i = 0; i<_faces.size(); i++)
	{
		int vi = _faces[i];
		int ni = _nFaces[i];
		int ti = _tFaces[i];

		if (normals[vi] == vec3())
		{
			normals[vi] = _normals[ni];
			tempFaces.push_back(vi);
			tempTFaces.push_back(ti);
		}
		else if (normals[vi] != _normals[ni])
		{
			vertices.push_back(vertices[vi]);
			normals.push_back(_normals[ni]);
			
			tempFaces.push_back(vertices.size() - 1);
			tempTFaces.push_back(ti);
		}
		else
		{
			tempFaces.push_back(vi);
			tempTFaces.push_back(ti);
		}
	}

	uvs.resize(vertices.size(), vec2(-1.f, -1.f));

	for (unsigned int i = 0; i < tempFaces.size(); i++)
	{
		int vi = tempFaces[i];
		int ti = tempTFaces[i];

		if (uvs[vi] == vec2(-1.f, -1.f))
		{
			uvs[vi] = _uvs[ti];
			faces.push_back(vi);
		}
		else if (uvs[vi] != uvs[ti])
		{
			vertices.push_back(vertices[vi]);
			normals.push_back(normals[vi]);
			uvs.push_back(uvs[ti]);

			faces.push_back(vi);
		}
		else
			faces.push_back(vi);
	}
}

bool MeshInfoLoader::loadModel(char *filename)
{
	FILE* f = fopen(filename, "r");
	if (f == NULL)
	{
		printf("File %s could not be opened\n", filename);
		return false;
	}

	//vector<GLv3f> vertices;
	vector<vec3> _normals;
	//vector<vec2> _uvs;
	vector<unsigned int> _faces;
	vector<unsigned int> _uvfaces;
	vector<unsigned int> _nfaces;

	while (true)
	{
		char text[256];

		int res = fscanf(f, "%s", text);
		if (res == EOF)
			break;

		if (strcmp(text, "v") == 0)
		{
			vec3 vert;

			fscanf(f, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
			vertices.push_back(vert);
		}
		else if (strcmp(text, "vn") == 0)
		{
			vec3 norm;

			fscanf(f, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
			_normals.push_back(norm);
		}
		else if (strcmp(text, "vt") == 0)
		{
			vec2 uv;

			fscanf(f, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(text, "f") == 0)
		{
			unsigned int vi1, vi2, vi3, ui1, ui2, ui3, ni1, ni2, ni3;

			if (fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vi1, &ui1, &ni1, &vi2, &ui2, &ni2, &vi3, &ui3, &ni3) != 9)
			{
				printf("Face couldn't be read\n");
				return false;
			}

			_faces.push_back(vi1 - 1);
			_faces.push_back(vi2 - 1);
			_faces.push_back(vi3 - 1);

			_uvfaces.push_back(ui1 - 1);
			_uvfaces.push_back(ui2 - 1);
			_uvfaces.push_back(ui3 - 1);

			_nfaces.push_back(ni1 - 1);
			_nfaces.push_back(ni2 - 1);
			_nfaces.push_back(ni3 - 1);
		}
	}

	sharedIndices(_faces, _nfaces, _normals, vertices, normals, indices);

	return true;
}

void MeshInfoLoader::clearData() {
	vertices.clear();
	normals.clear();
	indices.clear();
}

vec3 MeshInfoLoader::getCenter(){
	vec3 center;

	for (unsigned int i = 0; i<vertices.size(); i++)
	{
		center = center + vertices[i];
	}

	float divisor = (1.f / (vertices.size()));

	return center * divisor;
}

float MeshInfoLoader::getBottom(){
	float bottom = 1000.0;

	for (unsigned int i = 0; i<vertices.size(); i++)
	{
		bottom = min(bottom, vertices[i].z);
	}

	return bottom;
}

float MeshInfoLoader::getBoundingRadius()
{
	vec3 center = getCenter();
	float boundingRadius = 0.0;

	for (unsigned int i = 0; i<vertices.size(); i++)
	{
		vec3 point = vertices[i];
		float distance = length(center - point);
		boundingRadius = max(boundingRadius, distance);
	}

	return boundingRadius;
}

unsigned int MeshInfoLoader::LoadTexture(const char * filename) {
	unsigned int textureID = SOIL_load_OGL_texture
		(
		filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB
		);
	
	return textureID;
}