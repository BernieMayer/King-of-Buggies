#include "LevelInfoLoader.h"

#include <windows.h>
#include <fstream>

using namespace glm;

// Used to load .obj files which contain point locations of coins, powerup boxes, etc. (files have vertices only)
bool LevelInfoLoader::loadLevelData(char *filename)
{
	FILE* f = fopen(filename, "r");
	if (f == NULL)
	{
		printf("File %s could not be opened\n", filename);
		return false;
	}

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
			levelData.push_back(vert);
		}	
	}

	fclose(f);
	return true;
}

void LevelInfoLoader::clearData() {
	levelData.clear();
}

vector<vec3> LevelInfoLoader::getData() {
	return levelData;
}