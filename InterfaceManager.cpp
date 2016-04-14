#include "InterfaceManager.h"

ComponentInfo::ComponentInfo() :
	vertices(NULL),
	uvs(NULL),
	indices(NULL),
	anchorPoint(ANCHOR::TOP_LEFT),
	xOffset(0.f),
	yOffset(0.f),
	width(1.f),
	height(1.f),
	state(STATE::UP),
	isActive(true),
	displayFilter(DISPLAY::ALL),
	transform(1.f)
{
	for (unsigned int i = 0; i < 3; i++)
		texIDs[i] = NO_VALUE;
}

mat4 ComponentInfo::getMatrix(const mat4& winRatio)
{
	mat4 scale(1.f);
	scale[0][0] = width;
	scale[1][1] = height;

	vec2 anchor(xOffset / winRatio[0][0], yOffset / winRatio[1][1]);
	//vec2 anchor(xOffset, yOffset);

	vec2 corner;

	switch (anchorPoint)
	{
	case ANCHOR::TOP_LEFT:
		corner = vec2(-width, height)*0.5f;
		break;
	case ANCHOR::TOP_RIGHT:
		corner = vec2(width, height)*0.5f;
		break;
	case ANCHOR::BOTTOM_LEFT:
		corner = vec2(-width, -height)*0.5f;
		break;
	case ANCHOR::BOTTOM_RIGHT:
		corner = vec2(width, -height)*0.5f;
		break;
	case ANCHOR::CENTER:
		corner = vec2(0.f, 0.f)*0.5f;
		break;
	}

	vec2 translation = anchor - corner;
	mat4 transMat(1.f);
	transMat[3][0] = translation.x;
	transMat[3][1] = translation.y;

	return winRatio*transMat*scale*transform;
}


InterfaceManager::InterfaceManager() :winRatio(1.f), uiMat(Unshaded()) 
{
	initSquare();
}

InterfaceManager::InterfaceManager(unsigned int windowWidth, unsigned int windowHeight) : wWidth(windowWidth), wHeight(windowHeight), winRatio(1.f), uiMat(Unshaded())
{
	barHeight = wHeight / 100;

	updateWinRatio();

	initSquare();

}

void InterfaceManager::clear()
{
	components.clear();
}

void InterfaceManager::updateWinRatio()
{
	float minDim = min((float)wWidth, (float)wHeight);
	winRatio[0][0] = minDim / (float)wWidth;
	winRatio[1][1] = minDim / (float)wHeight;
}

void InterfaceManager::initSquare()
{
	float halfWidth = 0.5f;

	vec3 p00 (-halfWidth, -halfWidth, -0.99f);
	vec3 p10(halfWidth, -halfWidth, -0.99f);
	vec3 p01(-halfWidth, halfWidth, -0.99f);
	vec3 p11(halfWidth, halfWidth, -0.99f);

	vec2 uv00(0.f, 0.f);
	vec2 uv10(1.f, 0.f);
	vec2 uv01(0.f, 1.f);
	vec2 uv11(1.f, 1.f);

	squareVerts.push_back(p00);
	squareVerts.push_back(p10);
	squareVerts.push_back(p11);

	squareVerts.push_back(p00);
	squareVerts.push_back(p11);
	squareVerts.push_back(p01);

	squareUVs.push_back(uv00);
	squareUVs.push_back(uv10);
	squareUVs.push_back(uv11);

	squareUVs.push_back(uv00);
	squareUVs.push_back(uv11);
	squareUVs.push_back(uv01);
}

//  this is calculated based on the player's score
float InterfaceManager::getScoreBarWidth(GameState* state, unsigned int playerID) 
{
	float score = state->getPlayer(playerID)->getScore();
	
	float maxScore = state->getMaxScore();

	float barRatio = score / maxScore;
	float theWidth = (.35f * barRatio);
	
	return theWidth;
}

// this is predefined
unsigned int InterfaceManager::getScoreBarHeight() { return barHeight; }

void InterfaceManager::setWindowDim(int width, int height) 
{
	wWidth = width;
	wHeight = height;

	barHeight = 10;
}

void InterfaceManager::draw(unsigned int id, Renderer* r)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (components[id].isActive) {
		uiMat.useTextureShader();
		Viewport vp = r->getActiveViewport();

		uiMat.loadUniforms(components[id].getMatrix(winRatio*vp.viewRatio), mat4(1.f), vec3(), vec3(), components[id].texIDs[components[id].state], 1);

		r->loadVertUVBuffer(components[id].vertices, components[id].uvs);

		glDrawArrays(GL_TRIANGLES, 0, components[id].vertices->size());
	}

	glBindVertexArray(0);

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void InterfaceManager::drawAll(Renderer* r)
{
	drawAll(r, DISPLAY::ALL);
}

void InterfaceManager::drawAll(Renderer* r, unsigned int displayFilter)
{
	wWidth = r->getWidth();
	wHeight = r->getHeight();
	updateWinRatio();

	for (unsigned int i = 0; i < components.size(); i++)
	{
		unsigned int mask = components[i].displayFilter & displayFilter;
		if (components[i].displayFilter & (displayFilter | 1))	
			draw(i, r);
	}
}

/*vector<vector<vec3>> InterfaceManager::generateScoreBars(GameState* state) 
{
	vector<vector<vec3>> scorebarQuads;

	int numPlayers = state->numberOfPlayers();
	int barOffset = 0;
	
	for (int i = 0; i < numPlayers; i++) {
		float barWidth = getScoreBarWidth(state, i);
		vector<vec3> quadCoords;

		// Store the four corners of the scorebar
		quadCoords.push_back(vec3(0.f, barOffset, 0.f));
		quadCoords.push_back(vec3(barWidth, barOffset, 0.f));
		quadCoords.push_back(vec3(barWidth, (barOffset + 10), 0.f));
		quadCoords.push_back(vec3(0.f, (barOffset + 10), 0.f));
		
		scorebarQuads.push_back(quadCoords);
		barOffset += 10;
	}
	return scorebarQuads;
}*/

unsigned int InterfaceManager::generateComponentID()
{
	components.push_back(ComponentInfo());
	return components.size()-1;
}

void InterfaceManager::assignVertices(unsigned int id, vector<vec3>* vertices) { components[id].vertices = vertices; }

void InterfaceManager::assignUVs(unsigned int id, vector<vec2>* uvs) { components[id].uvs = uvs; }

void InterfaceManager::assignTexture(unsigned int id, unsigned int texID, unsigned int textureType)
{
	components[id].texIDs[textureType] = texID;
}

void InterfaceManager::assignIndices(unsigned int id, vector<unsigned int>* indices) { components[id].indices = indices; }

void InterfaceManager::assignSquare(unsigned int id)
{
	assignVertices(id, &squareVerts);
	assignUVs(id, &squareUVs);
}

void InterfaceManager::assignTransform(unsigned int id, const mat4& transform)
{
	components[id].transform = transform;
}

void InterfaceManager::setDisplayFilter(unsigned int id, unsigned int displayFilter)
{
	components[id].displayFilter = displayFilter;
}

void InterfaceManager::setDimensions(unsigned int id, float xOffset, float yOffset, float width, float height, unsigned int anchor)
{
	components[id].xOffset = xOffset;
	components[id].yOffset = yOffset;
	components[id].width = width;
	components[id].height = height;
	components[id].anchorPoint = anchor;
}

void InterfaceManager::toggleActive(unsigned int id, bool isActive)
{
	components[id].isActive = isActive;
}

vector<vec3>* InterfaceManager::storeVertices(vector<vec3>* vertices)
{
	vertexStorage.push_back(*vertices);
	return &vertexStorage.back();
}

vector<vec2>* InterfaceManager::storeUVs(vector<vec2>* uvs)
{
	uvStorage.push_back(*uvs);
	return &uvStorage.back();
}
