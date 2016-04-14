#ifndef INTERFACEMANAGER_H
#define INTERFACEMANAGER_H

#include "GameState.h"
#include "Renderer.h"
#include "Input.h"

class ComponentInfo {
public:
	vector<vec3>* vertices;
	vector<vec2>* uvs;
	vector<unsigned int>* indices;
	unsigned int texIDs[3];

	unsigned int displayFilter;

	unsigned int state;

	unsigned int anchorPoint;
	float xOffset, yOffset, width, height;
	
	mat4 transform;

	ComponentInfo();

	mat4 getMatrix(const mat4& winRatio);

	enum {UP_TEXTURE=0, OVER_TEXTURE, DOWN_TEXTURE};

	bool isActive;
};

enum DISPLAY{
	D1	= (1 << 1),
	D2	= (1 << 2),
	D3	= (1 << 3),
	D4	= (1 << 4),
	D5	= (1 << 5),
	D6	= (1 << 6),
	D7	= (1 << 7),
	D8	= (1 << 8),
	D9	= (1 << 9),
	ALL = D1 | D2 | D3 | D4 | D5 | D6 | D7 | D8 | D9
};

//Determines which corner UI elements are anchored from
enum ANCHOR {TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT, CENTER};

enum STATE {UP=0, OVER, DOWN};

class InterfaceManager {
private:
	unsigned int wHeight;
	unsigned int wWidth;
	unsigned int barHeight;

	vector<ComponentInfo> components;

	vector< vector<vec3> > vertexStorage;
	vector< vector<vec2> > uvStorage;

	vector<vec3> squareVerts;
	vector<vec2> squareUVs;

	mat4 winRatio;

	Unshaded uiMat;

	void initSquare();
	void updateWinRatio();

public:
	InterfaceManager();
	InterfaceManager(unsigned int windowWidth, unsigned int windowHeight);

	float getScoreBarWidth(GameState* state, unsigned int playerID);
	unsigned int getScoreBarHeight();

	void setWindowDim(int width, int height);
	unsigned int getWindowWidth() { return wWidth; }
	unsigned int getWindowHeight() { return wHeight; }

	mat4 getWinRatio() { return winRatio; }

	void clear();
	
	vector<vector<vec3>> generateScoreBars(GameState* state);

	//Component modification
	unsigned int generateComponentID();
	void assignVertices(unsigned int id, vector<vec3>* vertices);
	void assignUVs(unsigned int id, vector<vec2>* uvs);
	void assignSquare(unsigned int id);		//Uses vertices and uvs of unit square
	void assignTexture(unsigned int id, unsigned int texID, unsigned int textureType);
	void assignIndices(unsigned int id, vector<unsigned int>* indices);
	void setDisplayFilter(unsigned int id, unsigned int displayFilter);
	void assignTransform(unsigned int id, const mat4& transform);
	//Coordinates run from -1 to 1 (-1, -1 being bottom left corner)
	void setDimensions(unsigned int id, float xOffset, float yOffset, float width, float height, unsigned int anchor);
	void changeState(unsigned int id, unsigned int state);
	void toggleActive(unsigned int id, bool isActive);

	float getComponentWidth(int num) { return components[num].width; }
	float getComponentHeight(int num) { return components[num].height; }
	float getComponentX(int num) { return components[num].xOffset; }
	float getComponentY(int num) { return components[num].yOffset; }

	vector<vec3>* storeVertices(vector<vec3>* vertices);
	vector<vec2>* storeUVs(vector<vec2>* uvs);

	//Drawing elements
	void draw(unsigned int id, Renderer* r);
	void drawAll(Renderer* r);
	void drawAll(Renderer* r, unsigned int display);
};

#endif // INTERFACEMANAGER_H