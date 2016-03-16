#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

#include "GameState.h"
#include "Input.h"

class ComponentInfo {
private:
	vector<vec3>* vertices;
	vector<vec2>* uvs;
	unsigned int texIDs[3];

public:
	ComponentInfo();

	void setVertices(vector<vec3>* _vertices);
	void setUVs(vector<vec2>* _uvs);

	void setTexture(unsigned int texID, unsigned int textureType);

	enum {REST_TEXTURE=0, HIGHLIGHT_TEXTURE, PRESSED_TEXTURE};
};


class InterfaceManager {
private:
	unsigned int wHeight;
	unsigned int wWidth;
	unsigned int barHeight;

	vector<ComponentInfo> components;

	vector< vector<vec3> > vertexStorage;
	vector< vector<vec2> > uvStorage;

public:
	InterfaceManager();
	InterfaceManager(unsigned int windowWidth, unsigned int windowHeight);

	float getScoreBarWidth(GameState* state, unsigned int playerID);
	unsigned int getScoreBarHeight();

	void setWindowDim(int width, int height);
	
	vector<vector<vec3>> generateScoreBars(GameState* state);

	unsigned int generateComponentID();
	void assignVertices(unsigned int id, vector<vec3>* vertices);
	void assignUVs(unsigned int id, vector<vec2>* uvs);

	void storeVertices(vector<vec3>* vertices);
	void storeUVs(vector<vec2>* uvs);
};

#endif // INTERFACEMANAGER_H