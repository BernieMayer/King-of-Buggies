#ifndef MESHINFO_H
#define MESHINFO_H

#include "MeshObject.h"
#include "MeshInfoLoader.h"

#include "PxPhysicsAPI.h"

// Loads the indices of meshes contained in MeshInfo
enum { BRICK = 0, CUBE, WHEEL, COIN, MINE, BOMB, BOOST, DONUTLEVEL, PIPELEVEL, OLDLEVEL, BUGGY, UNITBUGGY, COUNT };

class MeshInfo {
public:
	MeshInfo();

	MeshObject getMesh(int objectType);
	MeshObject* getMeshPointer(int objectType);
	vector<vec3> getMeshVertices(int objectType);
	vector<vec3> getMeshNormals(int objectType);
	vector<vec2> getMeshUvs(int objectType);
	vector<unsigned int> getMeshIndices(int objectType);
	unsigned int getBuggyTexID(int index);

	unsigned int getRedBuggyTexID() { return redBuggyTexID; }
	unsigned int getBlueBuggyTexID() { return blueBuggyTexID; }
	unsigned int getGreenBuggyTexID() { return greenBuggyTexID; }
	unsigned int getOrangeBuggyTexID() { return orangeBuggyTexID; }
	unsigned int getPurpleBuggyTexID() { return purpleBuggyTexID; }
	unsigned int getGoldenBuggyTexID() { return goldenBuggyTexID; }

	unsigned int getUInitroID() { return UInitroID; }
	unsigned int getUIbombID() { return UIbombID; }
	unsigned int getUImineID() { return UImineID; }

	unsigned int getUIcomponentID(unsigned int itemType);
	unsigned int getCoinComponentID(unsigned int number);

	unsigned int getRedGoldenBuggy() { return redGoldenBuggy; }
	unsigned int getGreenGoldenBuggy() { return greenGoldenBuggy; }
	unsigned int getBlueGoldenBuggy() { return blueGoldenBuggy; }
	unsigned int getPurpleGoldenBuggy() { return purpleGoldenBuggy; }
	unsigned int getYouGoldenBuggy() { return youGoldenBuggy; }
	unsigned int getBuggyIndicator() { return buggyIndicator; }

	unsigned int getFullScoreBar() { return fullScoreBar; }
	unsigned int getScoreBar() { return scoreBar; }


	MeshInfoLoader meshLoader;

private:
	vector<MeshObject> meshes;
	unsigned int redBuggyTexID;
	unsigned int blueBuggyTexID;
	unsigned int greenBuggyTexID;
	unsigned int orangeBuggyTexID;
	unsigned int purpleBuggyTexID;
	unsigned int goldenBuggyTexID;

	vector<unsigned int> buggyTexIDs;

	unsigned int UInitroID;
	unsigned int UIbombID;
	unsigned int UImineID;

	vector<unsigned int> UIcomponentIDs;
	vector<unsigned int> coinComponentIDs;

	unsigned int redGoldenBuggy;
	unsigned int greenGoldenBuggy;
	unsigned int blueGoldenBuggy;
	unsigned int purpleGoldenBuggy;
	unsigned int youGoldenBuggy;
	unsigned int buggyIndicator;

	unsigned int fullScoreBar;
	unsigned int scoreBar;
};

#endif // MESHINFO_H