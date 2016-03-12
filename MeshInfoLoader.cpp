// File originally partially created by Troy Alderson for use in CPSC453 assignments. Reused with permission from Ben Stephenson

#include "MeshInfoLoader.h"
#include <windows.h>
#include <fstream>

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

/*int ObjFile::numVertices () {
return vertices.size() / 4;
}

GLv3f ObjFile::getVertex (int i) {
return GLv3f (vertices[3*i], vertices[3*i+1], vertices[3*i+2]);
}

GLv3f ObjFile::getNormal (int i) {
return GLv3f (normals[3*i], normals[3*i+1], normals[3*i+2]);
}

void ObjFile::setVertex (int i, GLv3f v) {
vertices[3*i] = v.x;
vertices[3*i+1] = v.y;
vertices[3*i+2] = v.z;
}

void ObjFile::setNormal (int i, GLv3f v) {
normals[3*i] = v.x;
normals[3*i+1] = v.y;
normals[3*i+2] = v.z;
}

void ObjFile::calculateNormals () {

printf("Calc normals\n");

for (int i = 0; i < vertices.size(); i += 3) {
normals.push_back (0.0f);
normals.push_back (0.0f);
normals.push_back (0.0f);
}

for (int i = 0; i < indices.size(); i += 3) {
int i1 = indices[i];
int i2 = indices[i+1];
int i3 = indices[i+2];

GLv3f p1 = getVertex (i1);
GLv3f p2 = getVertex (i2);
GLv3f p3 = getVertex (i3);

GLv3f fNormal = cross (p2 - p1, p3 - p1);

float l = length (fNormal);
if (fNormal.x != 0.0f && fNormal.y != 0.0f && fNormal.z != 0.0f) {
fNormal = normalize (fNormal);
setNormal (i1, getNormal(i1) + fNormal);
setNormal (i2, getNormal(i2) + fNormal);
setNormal (i3, getNormal(i3) + fNormal);
}
}

for (int i = 0; i < normals.size(); i += 3) {
setNormal (i/3, normalize(getNormal(i/3)));
}

printf("Calc normals->\n");
}


void ObjFile::flipFaces(){
for(int i=0; i<indices.size(); i+=3)
{
GLuint temp = indices[i];
indices[i] = indices[i+2];
indices[i+2] = temp;
}
}*/

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

/* This function is taken from a tutorial on cplusplus.com: http://www.cplusplus.com/articles/GwvU7k9E/
	Code reused with permission from Ben Stephenson. */
vector<unsigned char> MeshInfoLoader::LoadTexture(const char * filename)
{
	unsigned char* datBuff[2] = { nullptr, nullptr }; // Header buffers

	unsigned char* pixels = nullptr; // Pixels

	BITMAPFILEHEADER* bmpHeader = nullptr; // Header
	BITMAPINFOHEADER* bmpInfo = nullptr; // Info 

	// The file... We open it with it's constructor
	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		printf("Failure to open bitmap file.\n");
	
	}

	// Allocate byte memory that will hold the two headers
	datBuff[0] = new unsigned char[sizeof(BITMAPFILEHEADER)];
	datBuff[1] = new unsigned char[sizeof(BITMAPINFOHEADER)];

	file.read((char*)datBuff[0], sizeof(BITMAPFILEHEADER));
	file.read((char*)datBuff[1], sizeof(BITMAPINFOHEADER));

	// Construct the values from the buffers
	bmpHeader = (BITMAPFILEHEADER*)datBuff[0];
	bmpInfo = (BITMAPINFOHEADER*)datBuff[1];

	// First allocate pixel memory
	pixels = new unsigned char[bmpInfo->biSizeImage];

	// Go to where image data starts, then read in image data
	file.seekg(bmpHeader->bfOffBits);
	file.read((char*)pixels, bmpInfo->biSizeImage);

	// We're almost done. We have our image loaded, however it's not in the right format.
	// .bmp files store image data in the BGR format, and we have to convert it to RGB.
	// Since we have the value in bytes, this shouldn't be to hard to accomplish
	unsigned char tmpRGB = 0; // Swap buffer
	for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
	{
		tmpRGB = pixels[i];
		pixels[i] = pixels[i + 2];
		pixels[i + 2] = tmpRGB;
	}

	vector<unsigned char> imageVector;

	for (unsigned int j = 0; j < bmpInfo->biSizeImage; j += 3) {
		imageVector.push_back(pixels[j]);
		imageVector.push_back(pixels[j + 1]);
		imageVector.push_back(pixels[j + 2]);
	}
	return imageVector;
}
/* End of code from outside source */