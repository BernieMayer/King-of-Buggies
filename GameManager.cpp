#ifndef GAMEMANAGER_CPP
#define GAMEMANAGER_CPP

#include "GameManager.h"

GameManager::GameManager(GLFWwindow* newWindow) : renderer(newWindow), input(newWindow), state(), physics()
{
	for (unsigned int i = 0; i < MAX_PLAYERS; i++)
	{
		cam[i] = Camera(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 5.0), MODELVIEWER_CAMERA);
	}

	srand(time(NULL));
	window = newWindow;
	mat = Diffuse();
	shinyMat = Specular(20.f);
	tsMat = TorranceSparrow(15.f);
	matteMat = TorranceSparrow(0.5f);
	skyMaterial = Unshaded();
	shadowMat = Shadow();

	timePassedDecoy = 0;

	renderer.loadPerspectiveTransform(0.1f, 100.f, 90.f);		//Near, far, fov
	renderer.loadCamera(&cam[0]);

	ai = AIManager(&state);


	for (int i = 0; i < 5; i++) {
		smoothers.push_back(InputSmoother());
	}

	//TODO: Put this indexing somewhere useful;
	ai.initAI(1);

	// setup interface according to window dimensions
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	_interface = InterfaceManager(width, height);
//	_interface.setWindowDim(width, height);

	playerColours.push_back(0);
	playerColours.push_back(1);
	playerColours.push_back(2);
	playerColours.push_back(3);

	sound = SoundManager(state);
}


mat4 scaleMatrix(vec3 scale)
{
	mat4 scaleTransform(1.f);

	scaleTransform[0][0] = scale.x;
	scaleTransform[1][1] = scale.y;
	scaleTransform[2][2] = scale.z;

	return scaleTransform;
}

vec3 PxToVec3(PxVec3 vec) { return vec3(vec.x, vec.y, vec.z); }

void GameManager::cameraEnvironmentCollision(Camera* cam)
{
	cam->resetCameraDistance();

	vec3 origin = cam->getViewCenter();
	vec3 dir = cam->getPos() - origin;
	float maxDist = length(dir);
	dir = normalize(dir);

	float distance = physics.getEnvironmentRaycastResult(origin, dir, maxDist);
	if (distance > 0)
	{
		vec3 newPos = origin + dir*distance;
		cam->setPos(newPos);
	}
}

void GameManager::createPlayer(vec3 position, VehicleTraits traits, unsigned int texID)
{
	vec3 colour;
	// assign colour for this player's score bar
	if (texID == meshInfo.getRedBuggyTexID()) { colour = vec3(1.f, 0.f, 0.f); } // red
	else if (texID == meshInfo.getBlueBuggyTexID()) { colour = vec3(0.f, 0.f, 1.f); } // blue
	else if (texID == meshInfo.getGreenBuggyTexID()) { colour = vec3(0.f, 1.f, 0.f); } // green
	else if (texID == meshInfo.getOrangeBuggyTexID()) { colour = vec3(1.f, 0.64f, 0.f); } // orange
	else { colour = vec3(1.f, 0.f, 1.f); } // purple

	vehicleColours.push_back(colour);

	unsigned int chassisRenderID = renderer.generateObjectID();
	unsigned int physicsID = physics.vehicle_create(traits, position);

	MeshObject* playerMesh = meshInfo.getMeshPointer(BUGGY);
	renderer.assignMeshObject(chassisRenderID, playerMesh);
	renderer.assignMaterial(chassisRenderID, &tsMat);
	renderer.setShadowBehaviour(chassisRenderID, SHADOW_BEHAVIOUR::CAST | SHADOW_BEHAVIOUR::RECEIVE);
	if (state.numberOfPlayers() == 0) {
		renderer.assignTexture(chassisRenderID, meshInfo.getGoldenBuggyTexID());
	}
	else { renderer.assignTexture(chassisRenderID, texID); }

	MeshObject* wheelMesh = meshInfo.getMeshPointer(WHEEL);
	unsigned int wheelIDs[4];

	
	for (unsigned int i = 0; i < 4; i++)
	{
		wheelIDs[i] = renderer.generateObjectID();
		renderer.assignMeshObject(wheelIDs[i], wheelMesh);
		renderer.assignMaterial(wheelIDs[i], &matteMat);
		renderer.assignScale(wheelIDs[i], 
			scaleMatrix(vec3(traits.wheelWidth, traits.wheelRadius, traits.wheelRadius)));
		renderer.assignColor(wheelIDs[i], vec3(0.2f, 0.2f, 0.2f));
		//renderer.setShadowBehaviour(wheelIDs[i], SHADOW_BEHAVIOUR::CAST | SHADOW_BEHAVIOUR::RECEIVE);
	}

	state.addPlayer(PlayerInfo(chassisRenderID, physicsID, wheelIDs, colour, texID));
}

void GameManager::createDecoyGoldenBuggie(vec3 position, VehicleTraits traits)
{
	unsigned int chassisRenderID = renderer.generateObjectID();
	unsigned int physicsID = physics.vehicle_create(traits, position);

	vec3 colour = vehicleColours[state.numberOfPlayers()];
	unsigned int texID = meshInfo.getBuggyTexID(state.numberOfPlayers());

	MeshObject* playerMesh = meshInfo.getMeshPointer(BUGGY);
	renderer.assignMeshObject(chassisRenderID, playerMesh);
	renderer.assignMaterial(chassisRenderID, &tsMat);
	
	renderer.assignTexture(chassisRenderID, meshInfo.getGoldenBuggyTexID());
	

	MeshObject* wheelMesh = meshInfo.getMeshPointer(WHEEL);
	unsigned int wheelIDs[4];


	for (unsigned int i = 0; i < 4; i++)
	{
		wheelIDs[i] = renderer.generateObjectID();
		renderer.assignMeshObject(wheelIDs[i], wheelMesh);
		renderer.assignMaterial(wheelIDs[i], &matteMat);
		renderer.assignScale(wheelIDs[i],
			scaleMatrix(vec3(traits.wheelWidth, traits.wheelRadius, traits.wheelRadius)));
		renderer.assignColor(wheelIDs[i], vec3(0.2f, 0.2f, 0.2f));
	}


	//state.addPlayer(PlayerInfo(chassisRenderID, physicsID, wheelIDs, colour, texID));
	state.addDecoy(PlayerInfo(chassisRenderID, physicsID, wheelIDs, colour, texID ));
	smoothers.push_back(InputSmoother());

	
}

void GameManager::startDecoy(float time)
{
	timePassedDecoy = time;
}

float GameManager::decrementDecoyTimer(float time)
{
	timePassedDecoy -= time;
	return timePassedDecoy;
}


bool GameManager::isDecoyTimerUp()
{
	if (timePassedDecoy <  0)
	{
		timePassedDecoy = 0;
		return true;
	}
	return false;
}

void GameManager::createGroundPlane(vec3 normal, float offset)
{
	surfacePhysicsID = physics.ground_createPlane(vec3(0.f, 1.f, 0.f), 0.f);

	surfaceRenderID = renderer.generateObjectID();
	renderer.assignPlane(surfaceRenderID, 30.f, &surfaceVertices, &surfaceNormals, &surfaceIndices);
	renderer.assignMaterial(surfaceRenderID, &tsMat);
	renderer.assignColor(surfaceRenderID, vec3(0.5f, 0.5f, 0.5f));

}
void GameManager::createLevel(unsigned int objectID) {

	if (selectedLevel == 1) {
		levelMesh = meshInfo.getMeshPointer(DONUTLEVEL);
	}
	else if (selectedLevel == 0) {
		levelMesh = meshInfo.getMeshPointer(OLDLEVEL);
	}
	
	surfacePhysicsID = physics.ground_createGeneric(levelMesh);

	surfaceRenderID = renderer.generateObjectID();

	renderer.assignMeshObject(surfaceRenderID, levelMesh);
	renderer.assignMaterial(surfaceRenderID, &matteMat);
	renderer.assignColor(surfaceRenderID, vec3(0.5f, 0.5f, 0.5f));
	if (selectedLevel == 1) {
		renderer.assignTexture(surfaceRenderID, levelMesh->getTextureID());
		renderer.setShadowBehaviour(surfaceRenderID, SHADOW_BEHAVIOUR::CAST | SHADOW_BEHAVIOUR::RECEIVE);
	}
	

}

void GameManager::createBall(float radius)
{
	spherePhysicsID = physics.dynamic_createSphere(radius, vec3(0.f, 10.f, 0.f));

	sphereRenderID = renderer.generateObjectID();
	renderer.assignSphere(sphereRenderID, radius, 200, &ballVertices, &ballNormals, &ballUVs, &ballIndices);
	renderer.assignMaterial(sphereRenderID, &tsMat);
	renderer.assignTexture(sphereRenderID, 1);
	//renderer.assignColor(sphereRenderID, vec3(0.f, 0.f, 1.f));

	
}

// At least for now, coins are not being created as physics objects
void GameManager::createCoin(unsigned int coinIndex)
{
	Coin* newCoin = state.getCoin(coinIndex);
	MeshObject* coinMesh = meshInfo.getMeshPointer(COIN);

	unsigned int coin = renderer.generateObjectID();
	renderer.assignMeshObject(coin, coinMesh);
	renderer.assignMaterial(coin, &tsMat);
	renderer.assignColor(coin, vec3(1.f, 1.f, 0.f));
	renderer.setShadowBehaviour(coin, SHADOW_BEHAVIOUR::CAST);

	newCoin->setRenderID(coin);
}

void GameManager::createMine(unsigned int playerID)
{
	unsigned int powerup = renderer.generateObjectID();
	MeshObject* powerupMesh = meshInfo.getMeshPointer(MINE);

	renderer.assignMeshObject(powerup, powerupMesh);
	renderer.assignMaterial(powerup, &tsMat);
	renderer.assignTexture(powerup, powerupMesh->getTextureID());
	Mine newMine = Mine(3.0);
	
	vec3 playerPos = state.getPlayer(playerID)->getPos();
	vec3 playerForward = state.getPlayer(playerID)->getForward();

	// there is definitely a simpler way of doing this
	vec3 placementVec = playerPos - (playerForward);
	placementVec = placementVec - (playerForward);
	placementVec = placementVec - (playerForward);
	newMine.setPos(vec3((placementVec.x), (placementVec.y - 1), (placementVec.z)));
	newMine.setRenderID(powerup);
	state.addMine(newMine);
}

void GameManager::createBomb(unsigned int playerID)
{
	vec3 location = 5.0f * state.getPlayer(playerID)->getForward() + state.getPlayer(playerID)->getPos();
	Bomb b = Bomb(physics.createBomb(location, playerID), renderer.generateObjectID(), location);
	state.addPowerup(b);

	renderer.assignMeshObject(b.getRenderID(), meshInfo.getMeshPointer(BOMB));
	renderer.assignMaterial(b.getRenderID(), &tsMat);
	renderer.assignTexture(b.getRenderID(), meshInfo.getMeshPointer(BOMB)->getTextureID());
	state.pushEvent(new BombCreationEvent(location));
}

void GameManager::createPowerupBox(unsigned int objectID)
{
	unsigned int box = renderer.generateObjectID();
	MeshObject* boxMesh = meshInfo.getMeshPointer(CUBE);

	renderer.assignMeshObject(box, boxMesh);
	renderer.assignMaterial(box, &tsMat);
	renderer.assignTexture(box, boxMesh->getTextureID());
	renderer.setShadowBehaviour(box, SHADOW_BEHAVIOUR::CAST | SHADOW_BEHAVIOUR::RECEIVE);
	
	state.getPowerupBox(objectID)->setRenderID(box);
	physics.createPowerupBox(boxMesh->getVertices(), state.getPowerupBox(objectID)->getPos());
}

void GameManager::createBoostPad(unsigned int objectID)
{
	MeshObject* boostMesh = meshInfo.getMeshPointer(BOOST);
	unsigned int boost = renderer.generateObjectID();

	renderer.assignMeshObject(boost, boostMesh);
	renderer.assignMaterial(boost, &tsMat);
	renderer.assignColor(boost, vec3(0.f, 1.f, 0.f));
	//renderer.assignTexture

	state.getBoostPad(objectID)->setRenderID(boost);
}

void GameManager::initMenus() {
	sound.startMenuSong(state);
	
	int redIndex = 0;
	int greenIndex = 1;
	int blueIndex = 2;
	int purpleIndex = 3;

	int currentMenu = 0;
	int lastMenu = 2;

	unsigned int startScreen = LoadTexture("menus/KoB_StartScreen.bmp");
	unsigned int sScreen = _interface.generateComponentID();
	
	unsigned int levelSelectScreen = 0;
	unsigned int lScreen = 0;

	unsigned int carSelectScreen = 0;
	unsigned int cScreen = 0;

	unsigned int menuBackground = LoadTexture("menus/Background.bmp");
	unsigned int menu = _interface.generateComponentID();

	unsigned int p1Icon = 0;
	unsigned int Icon1 = 0;

	vector<float> xOffsets;
	vector<float> yOffsets;

	const float lSel1XBase = -0.55f;
	const float lSel1YBase = -0.89f;
	const float lSel2XBase = 0.525f;
	const float lSel2YBase = -0.89f;
	const float cSel1XBase = -0.79f;
	const float cSel2XBase = 0.25f;
	const float cSel1YBase = -0.22f;
	const float cSel2YBase = -0.93f;

	float lSel1X = lSel1XBase;
	float lSel1Y = lSel1YBase;
	float lSel2X = lSel2XBase;
	float lSel2Y = lSel2YBase;
	float cSel1X = cSel1XBase;
	float cSel2X = cSel2XBase;
	float cSel1Y = cSel1YBase;
	float cSel2Y = cSel2YBase;

	float xMod = 1.0f;
	float yMod = 1.0f;

	xOffsets.push_back(lSel1X);
	yOffsets.push_back(lSel1Y);

	float p1Scale = 0.25f;
	const float iconWidthBase = 0.145f;

	float iconWidth = iconWidthBase;

	xOffsets.push_back(cSel1X + iconWidth);
	xOffsets.push_back(cSel1X + iconWidth * 2);
	xOffsets.push_back(cSel1X + iconWidth * 3);
	yOffsets.push_back(cSel1Y);
	yOffsets.push_back(cSel1Y);
	yOffsets.push_back(cSel1Y);


	bool colourSelected[4] = { false, false, false, false };
	bool playerSelected[4] = { true, true, true, true };
	for (int i = 0; i < input.getNumPlayers(); i++) {
		playerSelected[i] = false;
	}

	// 0 = red, 1 = green, 2 = blue, 3 = purple
	int selectedColours[] = { 0, 1, 2, 3 };

	unsigned int p2Icon = 0;
	unsigned int Icon2 = 0;

	unsigned int p3Icon = 0;
	unsigned int Icon3 = 0;

	unsigned int p4Icon = 0;
	unsigned int Icon4 = 0;
	

	while (currentMenu <= lastMenu) {
		renderer.clearDrawBuffers(vec3(1.f, 1.f, 1.f));

		vector<Input> in;
		for (int i = 1; i <= input.getNumPlayers(); i++) {
			in.push_back(smoothers[i].smooth(input.getInput(i), false));
		}

		if (in[0].horn || glfwWindowShouldClose(window)) {
			_interface.clear();

			// Clear input so no input given for new menu on transition frame
			for (int i = 0; i < in.size(); i++) {
				in[i].jump = false;
				in[i].menu = false;
				in[i].powerup = false;
				in[i].tiltBackward = 0;
				in[i].tiltForward = 0;
				in[i].turnL = 0;
				in[i].turnR = 0;
			}

			sound.playDingSound();
			gameInit();

			gameLoop();
			return;
		}
		if ((in[0].menu || in[0].powerup || in[0].jump || in[0].horn || in[0].drift) && currentMenu == 0) {
			currentMenu++;
			_interface.clear();

			p1Icon = LoadTexture("menus/P1Icon.png");
			Icon1 = _interface.generateComponentID();

			levelSelectScreen = LoadTexture("menus/KoB_LevelScreen.bmp");
			lScreen = _interface.generateComponentID();

			menuBackground = LoadTexture("menus/Background.bmp");
			menu = _interface.generateComponentID();
		}
		else if (((in[0].jump && !in[0].isKeyboard) || (in[0].powerup && in[0].isKeyboard)) && currentMenu == 1) {
			currentMenu++;
			_interface.clear();

			if (xOffsets[0] == lSel1X) {
				selectedLevel = 0;
			}
			else if (xOffsets[0] == lSel2X) {
				selectedLevel = 1;
			}


			xOffsets[0] = cSel1X;
			yOffsets[0] = cSel1Y;
			p1Scale = 0.15f;
			p1Icon = LoadTexture("menus/P1Icon.png");
			Icon1 = _interface.generateComponentID();

			if (in.size() > 1) {
				p2Icon = LoadTexture("menus/P2Icon.png");
				Icon2 = _interface.generateComponentID();
			}

			if (in.size() > 2) {
				p3Icon = LoadTexture("menus/P3Icon.png");
				Icon3 = _interface.generateComponentID();
			}

			if (in.size() > 3) {
				p4Icon = LoadTexture("menus/P4Icon.png");
				Icon4 = _interface.generateComponentID();
			}

			carSelectScreen = LoadTexture("menus/KoB_CarScreen.bmp");
			cScreen = _interface.generateComponentID();

			menuBackground = LoadTexture("menus/Background.bmp");
			menu = _interface.generateComponentID();

			// Clear input so no input given for new menu on transition frame
			for (int i = 0; i < in.size(); i++) {
				in[i].jump = false;
				in[i].menu = false;
				in[i].powerup = false;
				in[i].tiltBackward = 0;
				in[i].tiltForward = 0;
				in[i].turnL = 0;
				in[i].turnR = 0;
			}
			
		}
		else if (in[0].menu && currentMenu == lastMenu && (playerSelected[0] && playerSelected[1] && playerSelected[2] && playerSelected[3])) {
			_interface.clear();

			// Clear input so no input given for new menu on transition frame
			for (int i = 0; i < in.size(); i++) {
				in[i].jump = false;
				in[i].menu = false;
				in[i].powerup = false;
				in[i].tiltBackward = 0;
				in[i].tiltForward = 0;
				in[i].turnL = 0;
				in[i].turnR = 0;
			}

			sound.playDingSound();
			gameInit();

			gameLoop();
			return;
		}

		if (currentMenu == 0) {
			_interface.assignSquare(sScreen);
			_interface.assignTexture(sScreen, startScreen, ComponentInfo::UP_TEXTURE);
			_interface.setDimensions(sScreen, 0.f, 0.f, 2.f, 2.f, ANCHOR::CENTER);
		}
		// if level select menu
		else if (currentMenu == 1) {
			if (xOffsets[0] == lSel1X && in[0].turnR < -0.3f) {
				xOffsets[0] = lSel2X;
			}
			else if (xOffsets[0] == lSel2X && in[0].turnL < -0.3f) {
				xOffsets[0] = lSel1X;
			}

			if (_interface.getWindowWidth() > _interface.getWindowHeight()) {
				xMod = (float)_interface.getWindowWidth() / (float)_interface.getWindowHeight();
				yMod = 1.0f;
			}
			else if (_interface.getWindowHeight() > _interface.getWindowWidth()) {
				xMod = 1.0f;
				yMod = (float)_interface.getWindowHeight() / (float)_interface.getWindowWidth();
			}
			else {
				xMod = 1.0f;
				yMod = 1.0f;
			}

			_interface.assignSquare(Icon1);
			_interface.assignTexture(Icon1, p1Icon, ComponentInfo::UP_TEXTURE);
			_interface.setDimensions(Icon1, xOffsets[0] / xMod, yOffsets[0] / yMod, p1Scale, p1Scale, ANCHOR::CENTER);

			_interface.assignSquare(lScreen);
			_interface.assignTexture(lScreen, levelSelectScreen, ComponentInfo::UP_TEXTURE);
			_interface.setDimensions(lScreen, 0.f, 0.f, 2.f, 2.f, ANCHOR::CENTER);
		}
		// If car select menu
		else if (currentMenu == 2) {

			if (xOffsets[0] == lSel1X && in[0].turnR < -0.3f) {
				xOffsets[0] = lSel2X;
			}
			else if (xOffsets[0] == lSel2X && in[0].turnL < -0.3f) {
				xOffsets[0] = lSel1X;
			}

			if (_interface.getWindowWidth() > _interface.getWindowHeight()) {
				xMod = (float)_interface.getWindowWidth() / (float)_interface.getWindowHeight();
				yMod = 1.0f;
			}
			else if (_interface.getWindowHeight() > _interface.getWindowWidth()) {
				xMod = 1.0f;
				yMod = (float)_interface.getWindowHeight() / (float)_interface.getWindowWidth();
			}
			else {
				xMod = 1.0f;
				yMod = 1.0f;
			}

			for (int i = 0; i < in.size(); i++) {

				// Only move if nothing is selected
				if (!playerSelected[i]) {
					if (yOffsets[i] == cSel1Y && in[i].tiltBackward > 0.3f)
					{
						yOffsets[i] = cSel2Y;
					}
					else if (yOffsets[i] == cSel2Y && in[i].tiltForward > 0.3f) {
						yOffsets[i] = cSel1Y;
					}
					else if (xOffsets[i] == cSel1X + iconWidth * i && in[i].turnR < -0.3f) {
						xOffsets[i] = cSel2X + iconWidth * i;
					}
					else if (xOffsets[i] == cSel2X + iconWidth * i && in[i].turnL < -0.3f) {
						xOffsets[i] = cSel1X + iconWidth * i;
					}
				}

				if (((in[i].jump && !in[i].isKeyboard) || (in[i].powerup && in[i].isKeyboard)) && !playerSelected[i]) {
					bool selectionOccured = false;
					if (xOffsets[i] == cSel1X + iconWidth * i && yOffsets[i] == cSel1Y && !colourSelected[0]) {
						selectedColours[i] = 0;
						colourSelected[0] = true;
						playerSelected[i] = true;
						selectionOccured = true;
					}
					else if (xOffsets[i] == cSel2X + iconWidth * i && yOffsets[i] == cSel1Y && !colourSelected[1]) {
						selectedColours[i] = 1;
						colourSelected[1] = true;
						playerSelected[i] = true;
						selectionOccured = true;
					}
					else if (xOffsets[i] == cSel1X + iconWidth * i && yOffsets[i] == cSel2Y && !colourSelected[2]) {
						selectedColours[i] = 2;
						colourSelected[2] = true;
						playerSelected[i] = true;
						selectionOccured = true;
					}
					else if (xOffsets[i] == cSel2X + iconWidth * i && yOffsets[i] == cSel2Y && !colourSelected[3]) {
						selectedColours[i] = 3;
						colourSelected[3] = true;
						playerSelected[i] = true;
						selectionOccured = true;
					}

					if (selectionOccured) {
						if (i == 0) {
							p1Icon = LoadTexture("menus/P1IconSel.png");
						}
						else if (i == 1) {
							p2Icon = LoadTexture("menus/P2IconSel.png");
						}
						else if (i == 2) {
							p3Icon = LoadTexture("menus/P3IconSel.png");
						}
						else if (i == 3) {
							p4Icon = LoadTexture("menus/P4IconSel.png");
						}

					}
				}
				else if (((in[i].powerup && !in[i].isKeyboard) || (in[i].drift && in[i].isKeyboard)) && playerSelected[i]) {
					playerSelected[i] = false;
					if (xOffsets[i] == cSel1X + iconWidth * i && yOffsets[i] == cSel1Y) {
						colourSelected[0] = false;
					}
					else if (xOffsets[i] == cSel2X + iconWidth * i && yOffsets[i] == cSel1Y) {
						colourSelected[1] = false;
					}
					else if (xOffsets[i] == cSel1X + iconWidth * i && yOffsets[i] == cSel2Y) {
						colourSelected[2] = false;
					}
					else if (xOffsets[i] == cSel2X + iconWidth * i && yOffsets[i] == cSel2Y) {
						colourSelected[3] = false;
					}

					if (i == 0) {
						p1Icon = LoadTexture("menus/P1Icon.png");
					}
					else if (i == 1) {
						p2Icon = LoadTexture("menus/P2Icon.png");
					}
					else if (i == 2) {
						p3Icon = LoadTexture("menus/P3Icon.png");
					}
					else if (i == 3) {
						p4Icon = LoadTexture("menus/P4Icon.png");
					}
				}


				if (selectedColours[i] == 0 && redIndex != i) {
					playerColours[redIndex] = playerColours[i];
					playerColours[i] = 0;

					if (blueIndex == i) {
						blueIndex = redIndex;
					}
					else if (greenIndex == i) {
						greenIndex = redIndex;
					}
					else if (purpleIndex == i) {
						purpleIndex = redIndex;
					}
					redIndex = i;
				}
				else if (selectedColours[i] == 1 && greenIndex != i) {
					playerColours[greenIndex] = playerColours[i];
					playerColours[i] = 1;

					if (redIndex == i) {
						redIndex = greenIndex;
					}
					else if (blueIndex == i) {
						blueIndex = greenIndex;
					}
					else if (purpleIndex == i) {
						purpleIndex = greenIndex;
					}
					greenIndex = i;
				}
				else if (selectedColours[i] == 2 && blueIndex != i) {
					playerColours[blueIndex] = playerColours[i];
					playerColours[i] = 2;

					if (redIndex == i) {
						redIndex = blueIndex;
					}
					else if (greenIndex == i) {
						greenIndex = blueIndex;
					}
					else if (purpleIndex == i) {
						purpleIndex = blueIndex;
					}

					blueIndex = i;
				}
				else if (selectedColours[i] == 3 && purpleIndex != i) {
					playerColours[purpleIndex] = playerColours[i];
					playerColours[i] = 3;

					if (redIndex == i) {
						redIndex = purpleIndex;
					}
					else if (greenIndex == i) {
						greenIndex = purpleIndex;
					}
					else if (blueIndex == i) {
						blueIndex = purpleIndex;
					}
					purpleIndex = i;
				}
			}

			_interface.assignSquare(Icon1);
			_interface.assignTexture(Icon1, p1Icon, ComponentInfo::UP_TEXTURE);
			_interface.setDimensions(Icon1, xOffsets[0] / xMod, yOffsets[0] / yMod, p1Scale, p1Scale, ANCHOR::CENTER);

			if (in.size() > 1) {
				_interface.assignSquare(Icon2);
				_interface.assignTexture(Icon2, p2Icon, ComponentInfo::UP_TEXTURE);
				_interface.setDimensions(Icon2, xOffsets[1] / xMod, yOffsets[1] / yMod, p1Scale, p1Scale, ANCHOR::CENTER);
			}

			if (in.size() > 2) {
				_interface.assignSquare(Icon3);
				_interface.assignTexture(Icon3, p3Icon, ComponentInfo::UP_TEXTURE);
				_interface.setDimensions(Icon3, xOffsets[2] / xMod, yOffsets[2] / yMod, p1Scale, p1Scale, ANCHOR::CENTER);
			}

			if (in.size() > 3) {
				_interface.assignSquare(Icon4);
				_interface.assignTexture(Icon4, p4Icon, ComponentInfo::UP_TEXTURE);
				_interface.setDimensions(Icon4, xOffsets[3] / xMod, yOffsets[3] / yMod, p1Scale, p1Scale, ANCHOR::CENTER);
			}

			_interface.assignSquare(cScreen);
			_interface.assignTexture(cScreen, carSelectScreen, ComponentInfo::UP_TEXTURE);
			_interface.setDimensions(cScreen, 0.f, 0.f, 2.f, 2.f, ANCHOR::CENTER);
		}

		_interface.assignSquare(menu);
		_interface.assignTexture(menu, menuBackground, ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(menu, 0.f, 0.f, 16.f, 8.f, ANCHOR::CENTER);

		
		_interface.drawAll(&renderer);

		sound.updateMenuSong(state);

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}
}

void GameManager::changeGoldenBuggy()
{
	physics.newGoldenBuggy = false;

	//Switch the golden buggie
	PlayerInfo* p = state.getPlayer(physics.indexOfGoldenBuggy);
	unsigned int chassisRenderId_goldenBuggy = p->getRenderID();
	renderer.assignTexture(chassisRenderId_goldenBuggy, meshInfo.getGoldenBuggyTexID());


	//Switch the player that used to be the golden buggy
	PlayerInfo* p_2 = state.getPlayer(physics.indexOfOldGoldenBuggy);
	int chasisRenderId_reg = p_2->getRenderID();
	// Switch buggy back to original colour
	unsigned int origTexture = p_2->getTextureID();
	renderer.assignTexture(chasisRenderId_reg, origTexture);

	state.setGoldenBuggy(physics.indexOfGoldenBuggy);

	switchBuggyUI();
}


void GameManager::processEvents()
{
	for (int i = 0; i < state.getNbEvents(); i++) {
		Event* e = state.getEvent(i);

		if (e->getType() == VEHICLE_BOMB_COLLISION_EVENT) {
			// Remove bomb
			VehicleBombCollisionEvent* powerupEvent = (VehicleBombCollisionEvent*)(e);
			Powerup* explodedBomb = state.getPowerup(powerupEvent->ob2);
			renderer.deleteDrawableObject(explodedBomb->getRenderID());
			state.removePowerup(powerupEvent->ob2);
		}
		else if (e->getType() == POWERUPBOX_COLLISION_EVENT)
		{
			PowerupCollisionEvent* powerupEvent = dynamic_cast<PowerupCollisionEvent*>(e);
			int vehicleId = powerupEvent->ob1;
			int powerupId = powerupEvent->ob2; //Delete the powerup, note the id is based on the order that the powerups are made.

			if (!state.getPowerupBox(powerupId)->getCollided()) {
				// temporarily remove powerup from board
				PowerupBox* collided = state.getPowerupBox(powerupId);
				vec3 pos = collided->getPos();
				vec3 newPos = pos;
				newPos.y = pos.y - 20;
				collided->setPos(newPos);
				collided->setCollided(true);
				collided->startCountdown();

				hasPowerup[vehicleId] = true;
				int powerUpType = randomPowerup();
				//if (!state.getPlayer(vehicleId)->isGoldenBuggy() && powerUpType == POWERUPS::DECOY)
				//powerUpType = POWERUPS::NITROBOOST;	//Prevents the non golden buggies from using the Decoy
				if (powerUpType == POWERUPS::NITROBOOST) {
					state.getPlayer(i)->setEnergyForNitro(300.0f);
					printf("Nitro Boost with energy level  %f \n", state.getPlayer(i)->getEnergyForNitro());
				}
				state.getPlayer(vehicleId)->addPowerUp(powerUpType);

				// display powerup information in HUD
				_interface.assignTexture(powerupComponentIDs[vehicleId], meshInfo.getUIcomponentID(powerUpType), ComponentInfo::UP_TEXTURE);
				_interface.toggleActive(powerupComponentIDs[vehicleId], true);

				sound.playPowerupSound(state.getPlayer(vehicleId)->getPos());
			}
		}
	}
}

void GameManager::checkCoinCollisions()
{
	// Check for player/coin collisions, and coin respawns also check for other collisions
	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		bool hasCoinCollision = state.checkCoinCollision(state.getPlayer(i)->getPos());
		bool hasBoostPadCollision = state.checkBoostPadCollision(state.getPlayer(i)->getPos());
		int hasMineCollision = state.checkMineCollision(state.getPlayer(i)->getPos());
		if (hasCoinCollision){
			//TODO change to all
			physics.modifySpeed(i, 0.3333f);
			sound.playDingSound(state.getPlayer(i)->getPos());
			state.getPlayer(i)->addCoin();
			_interface.assignTexture(playerCoinIDs[i], meshInfo.getCoinComponentID(state.getPlayer(i)->getNumCoins()), ComponentInfo::UP_TEXTURE);
		}

		if (hasBoostPadCollision){
			physics.applySpeedPadBoost(i);
		}

		if (hasMineCollision > -1){
			physics.applyMineExplosion(i);
			renderer.deleteDrawableObject(state.getMine(hasMineCollision)->getRenderID());
			state.removeMine(hasMineCollision);
			sound.playMineExplosionSound(state.getPlayer(i)->getPos());
		}
	}
	state.checkRespawns();
	state.applyRotations();
}

void GameManager::updateCamera(unsigned int i, Input input, float frameTime)
{
	//Update camera position
	PlayerInfo* activePlayer = state.getPlayer(i);
	vec4 cameraPosition = physics.vehicle_getGlobalPose(
		activePlayer->getPhysicsID())*vec4(0.f, 0.f, 0.f, 1.f);
	vec3 cPos = vec3(cameraPosition) / cameraPosition.w;

	if (cPos != cam[i].getViewCenter())
		cam[i].changeCenterAndPos(cPos - cam[i].getViewCenter());

	//Update to accomodate more players and multiple cameras
	cam[i].rotateView(input.camH*CAM_SENSITIVITY, input.camV*CAM_SENSITIVITY);

	cameraEnvironmentCollision(&cam[i]);

	//Track camera around front of vehicle
	vec4 carDir = physics.vehicle_getGlobalPose(activePlayer->getPhysicsID())*vec4(0.f, 0.f, 1.f, 0.f);
	if ((input.camH == 0.f) && (input.camV == 0.f))
		cam[i].trackDirAroundY(vec3(carDir), frameTime);
}

void GameManager::gameLoop()
{
	vector<vec3> polygons;
	vector<vec3> edges;
	
	mat4 lineTransform;
	lineTransform[3][1] = -2.f;

	NavMesh nav;
	if (selectedLevel == 1) {
		ai.nav.loadNavMesh("donutLevelNavMesh.obj");
	}
	else if (selectedLevel == 0) {
		ai.nav.loadNavMesh("HiResNavigationMesh.obj");
	}
	ai.nav.calculateImplicitEdges();
	ai.nav.navMeshToLines(&polygons, &edges);

	vector<vec3> path;
	
	clock.start();
	float timeProgressed = 0.f;
	unsigned int frameCount = 0;

	float frameTime = 1.f / 60.f;

	physics.startSim(frameTime);

	// 5 because of 4 players and the inital golden buggy which is not
	// a player
	Input inputs[MAX_PLAYERS];

	//Test code (Gives AI initial random path
	ai.testTarget = ai.getRandomTarget();
	
	if (!ai.findNewPath(1, state.getGoldenBuggy()->getPos(), false)) {
		//cout << endl;
	}

	vector<vec3> frontier;
	vector<vec3> paths;

	bool paused = false;
	bool firstFrame = true;

	unsigned int debugPathIterations = 0;
	unsigned int debuggedAI = 2;

	timeb loopStart = clock.getCurrentTime();

	hasPowerup.push_back(false);
	hasPowerup.push_back(false);
	hasPowerup.push_back(false);
	hasPowerup.push_back(false);

	unsigned int numScreens = input.getNumPlayers();
	//unsigned int numScreens = 4;
	renderer.splitScreenViewports(numScreens);

	unsigned int radarInterfaceID = _interface.generateComponentID();
	_interface.assignSquare(radarInterfaceID);
	//-1, -1 offset 0.5 width, 0.5 height. Screen space (-1, -1) to (1, 1)
	_interface.setDimensions(radarInterfaceID, -1.0f, -1.0f, 0.5f, 0.5f, ANCHOR::BOTTOM_LEFT);

	//use multiple of these to allow for split screen
	unsigned int radarFBO = renderer.createFramebuffer(400, 400);
	_interface.assignTexture(radarInterfaceID, renderer.getFramebufferTexture(radarFBO), ComponentInfo::UP_TEXTURE); //Magic third parameter

	while (!glfwWindowShouldClose(window) && !gameOver)
	{

		//Get inputs from players/ai
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++)
		{
			if ((state.getPlayer(i)->isAI()) && !paused)
			{
				frameCount++;
				if (frameCount > 30)
				{
					ai.findNewPath(i, state.getGoldenBuggy()->getPos(), !state.getPlayer(i)->isGoldenBuggy());
					frameCount = 0;
				}

				inputs[i] = ai.getInput(i, state);
			}
			else {
				inputs[i] = input.getInput(i + 1);
			}

			inputs[i] = smoothers[i].smooth(inputs[i], state.getPlayer(i)->getInAir());

			if(!paused)
				physics.handleInput(&inputs[i], state.getPlayer(i)->getPhysicsID());
		}

		if (inputs[0].menu && !firstFrame)
		{
			paused = !paused;

			if (paused)
			{
				freeCam = cam[0];
				freeCam.setCameraMode(FREEROAM_CAMERA);
				renderer.loadCamera(&freeCam);
				debugPathIterations = 0;
				pauseStartTime = clock.getCurrentTime();
			}
			else {
				renderer.loadCamera(&cam[0]);
				totalPausedTime = clock.getTimeSince(pauseStartTime);
			}
		}
		firstFrame = false;

		//Not AI code. AIManager shouldn't change the golden buggy
		if (physics.newGoldenBuggy){
			changeGoldenBuggy();
		}

		//Allow for nitro/powerup activation here
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
			if ((inputs[i].cheat_coin || inputs[i].powerup) && !paused) {

				if (hasPowerup.at(i))
				{
					applyPowerupEffect(i);
				}
			}
		}

		//Update game state and renderer
		physics.updateGameState(&state, frameTime);
		renderer.updateObjectTransforms(&state);
		sound.updateSounds(state, inputs);

		//Put into function
		processEvents();
		state.clearEvents();

		//Wait until end of frame
		float timeElapsed = clock.getElapsedTime();
		if (frameTime > timeElapsed)
			clock.waitUntil(frameTime - timeElapsed);

		//Display FPS
		timeProgressed += max(timeElapsed, frameTime);
		frameCount++;
		if (timeProgressed > 1.f)
		{
			printf("FPS = %d\n", frameCount);
			frameCount = 0;
			timeProgressed = 0.f;
		}

		clock.start();

		if (!paused) {
			checkCoinCollisions();

			physics.getSim();

			physics.startSim(max(timeElapsed, frameTime));
		}
		
		
		if (inputs[0].horn) {
			displayDebugging = !displayDebugging;
		}

		renderer.loadOptimizedBuffers();

		//Render shadow map
		if (USING_SHADOWS)
		{
			renderer.useFramebuffer(fbo);
			renderer.clearDrawBuffers(vec3(1.f, 1.f, 1.f));
			renderer.drawShadowMapAll(0);
			renderer.loadShadowMap(renderer.getFramebufferTexture(fbo));
			renderer.useFramebuffer(NO_VALUE);
		}
		//Draw scene
		renderer.clearDrawBuffers(vec3(1.f, 1.f, 1.f));

		for (unsigned int i = 0; i < numScreens; i++)
		{
			//Free camera movement
			if (paused && (i == 0))
			{
				renderer.loadCamera(&freeCam);
				//Debugging avoidance
				ai.debugAIPath(&paths, state.getGoldenBuggyID(), debugPathIterations);
				if (inputs[0].jump)
					debugPathIterations++;

				freeCam.rotateView(-inputs[0].camH*CAM_SENSITIVITY, -inputs[0].camV*CAM_SENSITIVITY);
				freeCam.move(vec3(inputs[0].turnL - inputs[0].turnR, 0, inputs[0].forward - inputs[0].backward));
			}
			else
			{
				updateCamera(i, inputs[i], frameTime);
				renderer.loadCamera(&cam[i]);
			}

			//Translate skydome
			mat4 translation;
			translation[3][0] = state.getPlayer(i)->getPos().x;
			translation[3][1] = state.getPlayer(i)->getPos().y;
			translation[3][2] = state.getPlayer(i)->getPos().z;
			renderer.assignTransform(skyboxID, translation);

			//Render
			renderer.useViewport(i+1);
				
			renderer.drawBufferedAll(USING_SHADOWS);

			renderer.drawUI(_interface.generateScoreBars(&state), vehicleColours);
			//Should be in a for loop for every player
			
			vector<vec2> radarPos = state.setupRadar(0);
			vector<vec3> coloursRadar;
			for (int i = 0; i < state.numberOfPlayers(); i++)
			{
				if (state.getPlayer(i)->isGoldenBuggy())
				{
					// 	rgb(255,215,0)
					coloursRadar.push_back(vec3(255.0f/255.0f, 215.0f/255.0f, 0.0));
				}
				else
				{
					coloursRadar.push_back(state.getPlayer(i)->getColour());
				}
			}
			//Draw the radar to the frameBuffer
			renderer.useFramebuffer(radarFBO);
			renderer.drawRadar(radarPos, coloursRadar);
			renderer.useFramebuffer(NO_VALUE);    //Reset to default fbo

			//renderer.drawRadar(state.setupRadarSeeingOnlyGoldenBuggy(0));

			//renderer.useViewport(i+1);

			//Debugging
			if (displayDebugging && (i == 0))
			{
				ai.getPathAsLines(state.getGoldenBuggyID(), &path);

				renderer.drawLines(polygons, vec3(0.f, 1.f, 0.f), lineTransform);
				renderer.drawLines(path, vec3(1.f, 0.f, 0.f), lineTransform);
				//renderer.drawLines(edges, vec3(0.f, 0.f, 1.f), lineTransform);
			}

			if (paused && (i == 0))
			{
				renderer.drawLines(paths, vec3(0.7f, 0.5f, 1.f), lineTransform);
			}

			_interface.drawAll(&renderer, 1 << (i+1));
		}
		

		
		
		if (!paused) {
			// increase score and check win conditions
			state.getGoldenBuggy()->incrementScore(clock.getTimeSince(lastScoreUpdateTime), totalPausedTime);
			lastScoreUpdateTime = clock.getCurrentTime();
			totalPausedTime = 0.f;

			unsigned int theScore = state.getGoldenBuggy()->getScore();

			if (theScore >= state.getMaxScore() && gameOver == false) {
				winner = state.getGoldenBuggyID();
				if (winner == 0) {
					sound.playWinSound(state.getPlayer(0)->getPos());
				}
				else {
					sound.playLossSound(state.getPlayer(0)->getPos());
				}
				gameOver = true;
				//break;
			}
		}

		renderer.useViewport(0);

		//Get path
		path.clear();

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

		
	}
	sound.pauseAllSounds();
	displayEndScreen(winner);

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GameManager::applyPowerupEffect(int playerNum)
{
	int powerUpId = state.getPlayer(playerNum)->usePowerUp();

	if (powerUpId == POWERUPS::NITROBOOST)
	{
		if (state.getPlayer(playerNum)->getEnergyForNitro() > 0.0f)
		{
			state.getPlayer(playerNum)->removeEnergyForNitro(30);
			physics.applyNitroBoost(playerNum);
			sound.playNitroSound(state.getPlayer(playerNum)->getPos());
			state.getPlayer(playerNum)->addPowerUp(POWERUPS::NITROBOOST);
			printf("Current energy level %f \n", state.getPlayer(playerNum)->getEnergyForNitro());
		}
		else {
			_interface.toggleActive(powerupComponentIDs[playerNum], false);
			hasPowerup.at(playerNum) = false;
		}
	}
	else if (powerUpId == POWERUPS::BOMB)
	{
		createBomb(playerNum);
	}
	else if (powerUpId == POWERUPS::MINE)
	{
		createMine(playerNum);
		sound.playMineCreationSound(state.getPlayer(playerNum)->getPos());
	}
	else if (powerUpId == POWERUPS::COIN)
	{
		physics.modifySpeed(playerNum, 0.3333f);	//Should change this..
		printf("Coin \n");
	}
	else if (powerUpId == POWERUPS::DECOY)
	{
		printf("Decoy \n");
		if (state.numOfDecoys >= 1) {
			//nothing or stuff dealing with multiple decoys
		}
		else {
			state.numOfDecoys += 1;
			//Apply decoy powerup logic
			VehicleTraits traits = VehicleTraits(physics.getMaterial());
			createPlayer(vec3(-5.f, 5.f, -15.f), traits, meshInfo.getGoldenBuggyTexID());
			//state.getPlayer(state.numberOfPlayers()).setAI(true);
			state.getPlayer(state.numberOfPlayers() - 1)->setDecoy(true);
			//also check the decoy timer here
			printf("Decoy? \n");
		}
	}

	if (powerUpId != POWERUPS::NITROBOOST) {
		_interface.toggleActive(powerupComponentIDs[playerNum], false);
		hasPowerup.at(playerNum) = false;
	}
}

void GameManager::gameInit()
{
	gameOver = false;
	

	if (selectedLevel == 1) {
		// temporary since we only have one level right now
		createLevel(DONUTLEVEL);
		MeshObject* levelMesh = meshInfo.getMeshPointer(DONUTLEVEL);
	}
	else if (selectedLevel == 0) {
		createLevel(OLDLEVEL);
		MeshObject* levelMesh = meshInfo.getMeshPointer(OLDLEVEL);
	}

	state.setMap(levelMesh, selectedLevel);

	for (int i = 0; i < playerColours.size(); i++) {
		if (playerColours[i] == 0) {
			vehicleColours.push_back(vec3(1.f, 0.f, 0.f)); // red car

		}
		else if (playerColours[i] == 1) {
			vehicleColours.push_back(vec3(0.f, 1.f, 0.f)); // green car
		}
		else if (playerColours[i] == 2) {
			vehicleColours.push_back(vec3(0.f, 0.f, 1.f)); // blue
		}
		else {
			// Black car sucks. it's purple now
			vehicleColours.push_back(vec3(1.f, 0.f, 1.f)); // purple
		}
	}
	
	// Yellow car also sucks, so now it's orange
	vehicleColours.push_back(vec3(1.f, 0.64f, 0.f)); // orange

	VehicleTraits traits = VehicleTraits(physics.getMaterial());
	traits.print();

	unsigned int TOO_MANY = 0;

	VehicleTraits temp = VehicleTraits(physics.getMaterial());
	traits.loadConfiguration("base");
	temp.print();

	vec3 playerPos = state.getStartCoord(0);

	if (playerColours[0] == 0) {
		createPlayer(playerPos, traits, meshInfo.getRedBuggyTexID());
	}
	else if (playerColours[0] == 1) {
		createPlayer(playerPos, traits, meshInfo.getGreenBuggyTexID());
	}
	else if (playerColours[0] == 2) {
		createPlayer(playerPos, traits, meshInfo.getBlueBuggyTexID());
	}
	else if (playerColours[0] == 3) {
		createPlayer(playerPos, traits, meshInfo.getPurpleBuggyTexID());
	}

	playerPos = state.getStartCoord(1);

	if (playerColours[1] == 0) {
		createPlayer(playerPos, traits, meshInfo.getRedBuggyTexID());
	}
	else if (playerColours[1] == 1) {
		createPlayer(playerPos, traits, meshInfo.getGreenBuggyTexID());
	}
	else if (playerColours[1] == 2) {
		createPlayer(playerPos, traits, meshInfo.getBlueBuggyTexID());
	}
	else if (playerColours[1] == 3) {
		createPlayer(playerPos, traits, meshInfo.getPurpleBuggyTexID());
	}

	playerPos = state.getStartCoord(2);

	if (playerColours[2] == 0) {
		createPlayer(playerPos, traits, meshInfo.getRedBuggyTexID());
	}
	else if (playerColours[2] == 1) {
		createPlayer(playerPos, traits, meshInfo.getGreenBuggyTexID());
	}
	else if (playerColours[2] == 2) {
		createPlayer(playerPos, traits, meshInfo.getBlueBuggyTexID());
	}
	else if (playerColours[2] == 3) {
		createPlayer(playerPos, traits, meshInfo.getPurpleBuggyTexID());
	}

	playerPos = state.getStartCoord(3);

	if (playerColours[3] == 0) {
		createPlayer(playerPos, traits, meshInfo.getRedBuggyTexID());
	}
	else if (playerColours[3] == 1) {
		createPlayer(playerPos, traits, meshInfo.getGreenBuggyTexID());
	}
	else if (playerColours[3] == 2) {
		createPlayer(playerPos, traits, meshInfo.getBlueBuggyTexID());
	}
	else if (playerColours[3] == 3) {
		createPlayer(playerPos, traits, meshInfo.getPurpleBuggyTexID());
	}

	for (unsigned int i = 0; i < TOO_MANY; i++)
	{
		createPlayer(vec3(5.f, 5.f, -15.f), traits, meshInfo.getBuggyTexID(0));
	}

	
	for (unsigned int i = input.getNumPlayers(); i < MAX_PLAYERS - 1; i++)
	{
		state.getPlayer(i)->setAI(true);
	}

	for (unsigned int i = 4; i < TOO_MANY + 4; i++)
	{
		state.getPlayer(i)->setAI(true);
	}



	state.setGoldenBuggy(0);
	sound.stopMenuSong();
	sound.initListener(state);
	sound.startSounds(state);



	// Add all coins as render objects
	for (unsigned int i = 0; i < state.numberOfCoins(); i++) { createCoin(i); }

	for (unsigned int i = 0; i < state.numberOfBoostPads(); i++) { createBoostPad(i); }

	for (unsigned int i = 0; i < state.numberOfPowerupBoxes(); i++) { createPowerupBox(i); }

	vec3 lightPos(60.f, 120.f, 60.f);
	unsigned int lightID = renderer.generateLightObject();
	renderer.setLightPosition(lightID, lightPos);

	ai.initAI();

	initUI();

	carSelectScreen = LoadTexture("menus/opacity-512.png");

	//Setup shadowmapping
	Camera tempCam(normalize(-lightPos), vec3(0.f, 1.f, 0.f), vec3(0.f));
	fbo = renderer.createDepthbuffer(2000, 2000);
	float n, f, w, h = 1.f;
	levelMesh->getFrustumBounds(tempCam.getDir(), tempCam.getRight(), tempCam.getUp(), &n, &f, &w, &h);
	//	renderer.positionLightCamera(0, levelMesh->getCenter(), levelMesh->getBoundingRadius());
	renderer.positionLightCamera(0, levelMesh->getCenter(), n, f, w, h);

	//Create skybox
	//skyboxTextureID = renderer.getFramebufferTexture(fbo);
	skyboxTextureID = LoadTexture("textures/sky_photo6.jpg");
	skyboxID = renderer.generateObjectID();
	renderer.assignSkyDome(skyboxID, 80.f, 50, &skyboxVerts, &skyboxUVs, &skyboxIndices, skyboxTextureID);
	renderer.assignMaterial(skyboxID, &skyMaterial);

	lastScoreUpdateTime = clock.getCurrentTime();
}

void GameManager::initUI()
{
	// setup for displaying each player's number of coins
	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) { // 10 = textures for numbers 0-9, plus the coin icon texture
		playerCoinIDs.push_back(_interface.generateComponentID());

		_interface.assignSquare(playerCoinIDs[i]);
		_interface.assignTexture(playerCoinIDs[i], meshInfo.getCoinComponentID(0), ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(playerCoinIDs[i], -.6f, 1.f, 0.3f, 0.15f, ANCHOR::TOP_LEFT);

		int display;
		if (i == 0) { display = DISPLAY::D1; }
		else if (i == 1) { display = DISPLAY::D2; }
		else if (i == 2) { display = DISPLAY::D3; }
		else if (i == 3) { display = DISPLAY::D4; }

		_interface.setDisplayFilter(playerCoinIDs[i], display);
		_interface.setDisplayFilter(playerCoinIDs[i], display);
	}

	// setup for displaying obtained powerups to the UI
	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		powerupComponentIDs.push_back(_interface.generateComponentID());

		_interface.assignSquare(powerupComponentIDs[i]);
		_interface.setDimensions(powerupComponentIDs[i], -1.f, 1.f, 0.5f, 0.5f, ANCHOR::TOP_LEFT);
		_interface.toggleActive(powerupComponentIDs[i], false);

		// set display filter for each player's indicator
		if (i == 0)
			_interface.setDisplayFilter(powerupComponentIDs[i], DISPLAY::D1);

		else if (i == 1)
			_interface.setDisplayFilter(powerupComponentIDs[i], DISPLAY::D2);

		else if (i == 2)
			_interface.setDisplayFilter(powerupComponentIDs[i], DISPLAY::D3);

		else if (i == 3)
			_interface.setDisplayFilter(powerupComponentIDs[i], DISPLAY::D4);
	}

	// setup UI that indicates which player is the golden buggy
	unsigned int display = _interface.generateComponentID();
	_interface.assignSquare(display);
	_interface.assignTexture(display, meshInfo.getBuggyIndicator(), ComponentInfo::UP_TEXTURE);
	_interface.setDimensions(display, 1.f, 1.f, .75f, 0.2f, ANCHOR::TOP_RIGHT);
	_interface.setDisplayFilter(display, DISPLAY::ALL);

	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		buggyIndicatorUIs.push_back(_interface.generateComponentID());
		_interface.assignSquare(buggyIndicatorUIs[i]);
		_interface.setDimensions(buggyIndicatorUIs[i], 1.f, .8f, .4f, 0.15f, ANCHOR::TOP_RIGHT);

		if (i == 0) { _interface.setDisplayFilter(buggyIndicatorUIs[i], DISPLAY::D1); }
		else if (i == 1) { _interface.setDisplayFilter(buggyIndicatorUIs[i], DISPLAY::D2); }
		else if (i == 2) { _interface.setDisplayFilter(buggyIndicatorUIs[i], DISPLAY::D3); }
		else if (i == 3) { _interface.setDisplayFilter(buggyIndicatorUIs[i], DISPLAY::D4); }
	}
	switchBuggyUI();

	totalPausedTime = 0.f;

	//Add dummy objects to interface
	carSelectScreen = LoadTexture("menus/opacity-512.png");
}

void GameManager::switchBuggyUI() {
	unsigned int golden = state.getGoldenBuggyID();
	vec3 goldenColour = state.getPlayer(golden)->getColour();
	int texID;
	// red is golden buggy
	if (goldenColour == vec3(1.f, 0.f, 0.f)) { texID = meshInfo.getRedGoldenBuggy(); }
	else if (goldenColour == vec3(0.f, 1.f, 0.f)) { texID = meshInfo.getGreenGoldenBuggy(); }
	else if (goldenColour == vec3(0.f, 0.f, 1.f)) { texID = meshInfo.getBlueGoldenBuggy(); }
	else if (goldenColour == vec3(1.f, 0.f, 1.f)) { texID = meshInfo.getPurpleGoldenBuggy(); }

	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		if (i != golden) {
			_interface.assignTexture(buggyIndicatorUIs[i], texID, ComponentInfo::UP_TEXTURE);
		}
		else {
			_interface.assignTexture(buggyIndicatorUIs[i], meshInfo.getYouGoldenBuggy(), ComponentInfo::UP_TEXTURE);
		}
	}
}

int GameManager::randomPowerup()
{
	/*
	1 - Nitro Boost
	2 - Bomb 
	3 - Mine
	4 - Coin?
	5 - Decoy?

	*/
	

	int powerupId = rand() % 3;
	return powerupId;
}

void GameManager::displayEndScreen(unsigned int winnerID)
{
	_interface.clear();

	const float xOffset1Base = -0.625f;
	const float xOffset2Base = 0.6f;
	const float yOffset1Base = -0.9f;
	const float yOffset2Base = -0.9f;

	float xOffset = xOffset1Base;
	float yOffset = yOffset1Base;

	float xMod = 1.0f;
	float yMod = 1.0f;

	unsigned int winnerTexture;
	if (winner == 0) {
		winnerTexture = LoadTexture("menus/Player1.png");
	}
	else if (winner == 1) {
		winnerTexture = LoadTexture("menus/Player2.png");
	}
	else if (winner == 2) {
		winnerTexture = LoadTexture("menus/Player3.png");
	}
	else {
		winnerTexture = LoadTexture("menus/Player4.png");
	}
	unsigned int winnerIcon = _interface.generateComponentID();

	unsigned int p1Texture = LoadTexture("menus/P1Icon.png");
	unsigned int p1Icon = _interface.generateComponentID();

	unsigned int endScreenTexture = LoadTexture("menus/KoB_EndScreen.bmp");
	unsigned int endScreen = _interface.generateComponentID();

	unsigned int menuBackground = LoadTexture("menus/Background.bmp");
	unsigned int menu = _interface.generateComponentID();


	Input in = input.getInput(1);
	bool doneLoop = false;
	while (!doneLoop) {
		renderer.clearDrawBuffers(vec3(1.f, 1.f, 1.f));

		if (glfwWindowShouldClose(window)) {
			return;
		}

		if (_interface.getWindowWidth() > _interface.getWindowHeight()) {
			xMod = (float)_interface.getWindowWidth() / (float)_interface.getWindowHeight();
			yMod = 1.0f;
		}
		else if (_interface.getWindowHeight() > _interface.getWindowWidth()) {
			xMod = 1.0f;
			yMod = (float)_interface.getWindowHeight() / (float)_interface.getWindowWidth();
		}
		else {
			xMod = 1.0f;
			yMod = 1.0f;
		}

		Input in = input.getInput(1);
		if (in.turnL < -0.3 && xOffset == xOffset2Base) {
			xOffset = xOffset1Base;
			yOffset = yOffset1Base;
		}
		else if (in.turnR < -0.3 && xOffset == xOffset1Base) {
			xOffset = xOffset2Base;
			yOffset = yOffset2Base;
		}

		_interface.assignSquare(p1Icon);
		_interface.assignTexture(p1Icon, p1Texture, ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(p1Icon, xOffset / xMod, yOffset / yMod, 0.25, 0.25, ANCHOR::CENTER);

		_interface.assignSquare(winnerIcon);
		_interface.assignTexture(winnerIcon, winnerTexture, ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(winnerIcon, 0.0f, 0.2f, 1, 0.268f, ANCHOR::CENTER);

		_interface.assignSquare(endScreen);
		_interface.assignTexture(endScreen, endScreenTexture, ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(endScreen, 0.0f, 0.0f, 2, 2, ANCHOR::CENTER);

		_interface.assignSquare(menu);
		_interface.assignTexture(menu, menuBackground, ComponentInfo::UP_TEXTURE);
		_interface.setDimensions(menu, 0.f, 0.f, 16.f, 8.f, ANCHOR::CENTER);

		if (in.jump && !in.isKeyboard) {
			doneLoop = true;
		}
		else if (in.powerup && in.isKeyboard) {
			doneLoop = true;
		}

		_interface.drawAll(&renderer);

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}

	if (xOffset == xOffset1Base) {
		_interface.clear();
		partialTeardown();
		initMenus();
	}
	else {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void GameManager::partialTeardown() {
	vehicleColours.clear();

	surfaceVertices.clear();		//TEMPORARY
	surfaceNormals.clear();		//TEMPORARY
	surfaceIndices.clear();	//TEMPORARY

	ballVertices.clear();			//TEMPORARY
	ballNormals.clear();			//TEMPORARY
	ballIndices.clear();	//TEMPORARY
	ballUVs.clear();				//TEMPORARY


	//Skybox - Temporary
	skyboxVerts.clear();
	skyboxUVs.clear();
	skyboxIndices.clear();

	hasPowerup.clear();

	playerColours.clear();

	powerupComponentIDs.clear();

	playerCoinIDs.clear();

	selectedLevel = 1;

	state.clear();
	physics.clear();
	ai.clear();
	_interface.clear();
	sound.deleteAll();
}

#endif // GAMEMANAGER_CPP