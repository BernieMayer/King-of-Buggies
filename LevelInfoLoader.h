#ifndef LEVELINFOLOADER_H
#define LEVELINFOLOADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

class LevelInfoLoader {
public:
	LevelInfoLoader() {};
	bool loadLevelData(char *filename);
	void clearData();

	vector<vec3> getData();

private:
	vector<vec3> levelData;
};

#endif // LEVELINFOLOADER_H