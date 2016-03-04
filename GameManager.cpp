#ifndef GAMEMANAGER_CPP
#define GAMEMANAGER_CPP

#include "GameManager.h"


GameManager::GameManager(GLFWwindow* newWindow) : renderer(newWindow), input(newWindow), state(), physics(), 
	cam(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 5.0), MODELVIEWER_CAMERA)
{
	window = newWindow;
	mat = Diffuse();
	shinyMat = Specular(20.f);
	tsMat = TorranceSparrow(10.f);
	matteMat = TorranceSparrow(0.5f);

	renderer.loadPerspectiveTransform(0.1f, 50.f, 90.f);		//Near, far, fov
	renderer.loadCamera(&cam);

	ai = AIManager(&state);

	//TODO: Put this indexing somewhere useful;
	ai.initAI(1);

	gameInit();
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

void GameManager::createPlayer(vec3 position, VehicleTraits traits)
{
	//Make argument to function later

	unsigned int chassisRenderID = renderer.generateObjectID();
	unsigned int physicsID = physics.vehicle_create(traits, position);

	MeshObject* playerMesh = meshInfo.getMeshPointer(BUGGY);
	renderer.assignMeshObject(chassisRenderID, playerMesh);
	renderer.assignMaterial(chassisRenderID, &tsMat);
	//renderer.assignScale(chassisRenderID, scaleMatrix(PxToVec3(traits.chassisDims)*0.5f));
	if (state.numberOfPlayers() == 0)
		renderer.assignColor(chassisRenderID, vec3(1.0f, 0.84f, 0.0f));
	else
		renderer.assignColor(chassisRenderID, vec3(1.0, 0.0f, 0.0f));
	//renderer.assignColor(chassisRenderID, vec3(1.f, 0.f, 0.f));

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

	state.addPlayer(PlayerInfo(chassisRenderID, physicsID, wheelIDs));
	

}

void GameManager::createAI(vec3 position)
{
	VehicleTraits traits = VehicleTraits(physics.getMaterial());	//physics.createMaterial(0.5f, 0.5f, 0.5f));		//Make argument to function later

	unsigned int chassisRenderID = renderer.generateObjectID();
	unsigned int physicsID = physics.vehicle_create(traits, position);

	MeshObject* playerMesh = meshInfo.getMeshPointer(CUBE);
	renderer.assignMeshObject(chassisRenderID, playerMesh);
	renderer.assignMaterial(chassisRenderID, &tsMat);
	renderer.assignScale(chassisRenderID, scaleMatrix(PxToVec3(traits.chassisDims)*0.5f));
	renderer.assignColor(chassisRenderID, vec3(1.f, 0.f, 0.f));

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

	state.addAI(PlayerInfo(chassisRenderID, physicsID, wheelIDs));
}

void GameManager::createGroundPlane(vec3 normal, float offset)
{
	surfacePhysicsID = physics.ground_createPlane(vec3(0.f, 1.f, 0.f), 0.f);

	surfaceRenderID = renderer.generateObjectID();
	renderer.assignPlane(surfaceRenderID, 30.f, &surfaceVertices, &surfaceNormals, &surfaceIndices);
	renderer.assignMaterial(surfaceRenderID, &matteMat);
	renderer.assignColor(surfaceRenderID, vec3(0.5f, 0.5f, 0.5f));

}
void GameManager::createTestLevel() {
	MeshObject* levelMesh = meshInfo.getMeshPointer(LEVEL);
	surfacePhysicsID = physics.ground_createGeneric(levelMesh);

	surfaceRenderID = renderer.generateObjectID();

	renderer.assignMeshObject(surfaceRenderID, levelMesh);
	renderer.assignMaterial(surfaceRenderID, &tsMat);
	renderer.assignColor(surfaceRenderID, vec3(0.5f, 0.5f, 0.5f));

}

void GameManager::createBall(float radius)
{
	spherePhysicsID = physics.dynamic_createSphere(radius, vec3(0.f, 10.f, 0.f));

	sphereRenderID = renderer.generateObjectID();
	renderer.assignSphere(sphereRenderID, radius, 200, &ballVertices, &ballNormals, &ballIndices);
	renderer.assignMaterial(sphereRenderID, &tsMat);
	renderer.assignColor(sphereRenderID, vec3(0.f, 0.f, 1.f));

	
}

void GameManager::addCoin(int playerId)
{
	//Add a coin into the player properties here

	PlayerInfo* p = state.getPlayer(playerId);

	int physID = p->getPhysicsID();
}

// At least for now, coins are not being created as physics objects
void GameManager::createCoin(vec3 position)
{
	Coin newCoin;
	MeshObject* coinMesh = meshInfo.getMeshPointer(COIN);

	unsigned int coin = renderer.generateObjectID();
	renderer.assignMeshObject(coin, coinMesh);
	renderer.assignMaterial(coin, &tsMat);
	renderer.assignColor(coin, vec3(1.f, 1.f, 0.f));

	newCoin.setRenderID(coin);
	newCoin.setPos(position);

	state.addCoin(newCoin);
}

void GameManager::gameLoop()
{
	vector<vec3> polygons;
	vector<vec3> edges;
	
	mat4 lineTransform;
	lineTransform[3][1] = -6.f;

	NavMesh nav;
	ai.nav.loadNavMesh("HiResNavigationMesh.obj");
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
	Input inputs[5];

	//Test code (Gives AI initial random path
	ai.testTarget = ai.getRandomTarget();
	
	if (!ai.findNewPath(1, ai.testTarget))
		cout << endl;

	while (!glfwWindowShouldClose(window))
	{
		/*
		Input in = input.getInput(1);		//Get input
		inputs[0] = in;
		

		if (in.cheat_coin)
			std::cout << "Cheated in a coin \n";
			*/
		

		//Get inputs from players/ai
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++)
		{
			if (state.getPlayer(i)->isAI())
			{
				frameCount++;
				if (frameCount > 30)
				{
					ai.findNewPath(i, ai.testTarget);
					frameCount = 0;
				}

				inputs[i] = ai.getInput(i);
			}
			else
				inputs[i] = input.getInput(i + 1);

			physics.handleInput(&inputs[i], state.getPlayer(i)->getPhysicsID());
		}

	
		/*if (state.numberOfPlayers() > 1){

			//Change this to AI code

			vec3 testPos;
			testPos.x = -10.0;
			testPos.y = 0.5f;
			testPos.z = -15.0f;
			Input ai_in = ai.updateAI(0, in.menu, testPos); //Test code?
			if (ai.atPoint(0, testPos)) {
				cout << "At point\n";
			}
			inputs[1] = ai_in;

			physics.handleInput(&ai_in, state.getPlayer(1)->getPhysicsID());


			//Not AI code. AIManager shouldn't change the golden buggy
			if (physics.newGoldenBuggy){
				physics.newGoldenBuggy = false;

				//Switch the golden buggie
				PlayerInfo* p = state.getPlayer(physics.indexOfGoldenBuggy);
				unsigned int chasisRenderId_goldenBuggy = p->getRenderID();
				renderer.assignColor(chasisRenderId_goldenBuggy, vec3(1.0f, 0.84f, 0.0f));


				//Switch the player that used to be the golden buggy
				PlayerInfo* p_2 = state.getPlayer(physics.indexOfOldGoldenBuggy);
				int chasisRenderId_reg = p_2->getRenderID();
				renderer.assignColor(chasisRenderId_reg, vec3(1.0f, 0.0f, 0.0f));
				
			}
		}*/

		//Not AI code. AIManager shouldn't change the golden buggy
		if (physics.newGoldenBuggy){
			physics.newGoldenBuggy = false;

			//Switch the golden buggie
			PlayerInfo* p = state.getPlayer(physics.indexOfGoldenBuggy);
			unsigned int chasisRenderId_goldenBuggy = p->getRenderID();
			renderer.assignColor(chasisRenderId_goldenBuggy, vec3(1.0f, 0.84f, 0.0f));


			//Switch the player that used to be the golden buggy
			PlayerInfo* p_2 = state.getPlayer(physics.indexOfOldGoldenBuggy);
			int chasisRenderId_reg = p_2->getRenderID();
			renderer.assignColor(chasisRenderId_reg, vec3(1.0f, 0.0f, 0.0f));
		}

		//Physics sim 
		physics.getSim();

		physics.startSim(frameTime);

		float scale = 0.1f;

		//Update to accomodate more players and multiple cameras
		if (!inputs[0].drift)//in.powerup)
			cam.rotateView(inputs[0].camH*scale, inputs[0].camV*scale);
		if (inputs[0].drift)
			cam.zoom(inputs[0].camV*0.95f + 1.f);

		

		//Update game state and renderer
		physics.updateGameState(&state, frameTime);
		renderer.updateObjectTransforms(&state);
		sound.updateSounds(state, inputs);

		//Test code...
		PlayerInfo* player = state.getPlayer(0);
		vec3 posPlayer = player->getPos();

		PlayerInfo* ai_state = state.getPlayer(1);
		vec3 posAI = ai_state->getPos();

		vec3 diff = posAI - posPlayer;

		//Update sphere -- TEMPORARY
		renderer.assignTransform(sphereRenderID, physics.dynamic_getGlobalPose(spherePhysicsID));


		// Check for player/coin collisions, and coin respawns
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
			bool hasCoinCollision = state.checkCoinCollision(state.getPlayer(i)->getPos());
			if (hasCoinCollision){
				physics.modifySpeed(i, 0.3333f);
			}
		}

		//Update camera position
		PlayerInfo* activePlayer = state.getPlayer(0);
		vec4 cameraPosition = physics.vehicle_getGlobalPose(
			activePlayer->getPhysicsID())*vec4(0.f, 0.f, 0.f, 1.f);
		vec3 cPos = vec3(cameraPosition) / cameraPosition.w;

		if (cPos != cam.getViewCenter())
			cam.changeCenterAndPos(cPos - cam.getViewCenter());


		//Track camera around front of vehicle
		vec4 carDir = physics.vehicle_getGlobalPose(activePlayer->getPhysicsID())*vec4(0.f, 0.f, 1.f, 0.f);

		cam.trackDirAroundY(vec3(carDir), 1.f / 60.f);

		//Draw scene
		renderer.clearDrawBuffers();
		renderer.drawAll();

		//Get path
		path.clear();
		//ai.nav.getPathLines(&path, state.getPlayer(0)->getPos(), state.getPlayer(1)->getPos());

		ai.getPathAsLines(1, &path);
		vector<vec3> carPos;
		vec3 p1 = state.getPlayer(0)->getPos();
		vec3 p2 = state.getPlayer(1)->getPos();
		p1.y = polygons[0].y;
		p2.y = p1.y;
		carPos.push_back(p1);
		//carPos.push_back(p2);
		carPos.push_back(ai.getCurrentTarget(1));

		//Debugging
		renderer.drawLines(polygons, vec3(0.f, 1.f, 0.f), lineTransform);
		renderer.drawLines(path, vec3(1.f, 0.f, 0.f), lineTransform);
		renderer.drawLines(edges, vec3(0.f, 0.f, 1.f), lineTransform);
		renderer.drawPoints(carPos, vec3(1.f, 0.f, 0.f), lineTransform);

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

		//Wait until end of frame
		float timeElapsed = clock.getElapsedTime();
		if (frameTime > timeElapsed)
			clock.waitUntil(frameTime - timeElapsed);
		
		//Display FPS
		timeProgressed += max(timeElapsed, frameTime);
		frameCount++;
		if (timeProgressed > 1.f)
		{
			//printf("FPS = %d\n", frameCount);
			frameCount = 0;
			timeProgressed = 0.f;
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

void GameManager::gameInit()
{
	initTestScene();




}

void GameManager::initTestScene()
{
	VehicleTraits traits = VehicleTraits(physics.getMaterial());
	traits.print();


	VehicleTraits temp = VehicleTraits(physics.getMaterial());
	traits.loadConfiguration("base");
	temp.print();

	createPlayer(vec3(0.f, 5.f, 0.f), traits);
	createPlayer(vec3(-5.f, 5.f, -15.f), traits);
	state.getPlayer(1)->setAI(true);
	//createGroundPlane(vec3(0.f, 1.f, 0.f), 0.f);
	createTestLevel();
	createBall(0.5f);

	state.setGoldenBuggy(0);

	createCoin(vec3(10.f, -0.3f, 10.f));
	createCoin(vec3(10.f, -0.3f, -10.f));

	vec3 lightPos(60.f, 60.f, 60.f);
	unsigned int lightID = renderer.generateLightObject();
	renderer.setLightPosition(lightID, lightPos);

	ai.initAI();
	sound = SoundManager(state);
	
	//createPlayer(vec3(0.f, 5.f, 3.f)); //SHOULD BE AI methods

	
}

void quitGame()
{

}



/*
void GameManager::createPowerups()
{
int i = 0;
while (i < NUM_POWERUPS) {
unsigned int powerup = renderer.generateObjectID();
state.getPowerup(i)->setRenderID(powerup);

// leave commented for now, there's no powerup meshes
//MeshObject coinMesh = meshInfo.getMesh(POWERUP);
MeshObject powerupMesh = meshInfo.getMesh(SPHERE);
vector<vec3> powerupVerts = powerupMesh.getVertices();
vector<vec3> powerupNormals = powerupMesh.getNormals();
vector<unsigned int> powerupIndices = powerupMesh.getIndices();

renderer.assignMesh(powerup, &powerupVerts);
renderer.assignNormals(powerup, &powerupNormals);
renderer.assignIndices(powerup, &powerupIndices);
renderer.assignMaterial(powerup, &tsMat);
i++;
}
}

void GameManager::createPowerupBoxes()
{
int i = 0;
while (i < NUM_POWERBOXES) {
unsigned int box = renderer.generateObjectID();
state.getPowerupBox(i)->setRenderID(box);

// leave commented for now, there's no powerup meshes
//MeshObject coinMesh = meshInfo.getMesh(POWERUP);
MeshObject boxMesh = meshInfo.getMesh(SPHERE);
vector<vec3> boxVerts = boxMesh.getVertices();
vector<vec3> boxNormals = boxMesh.getNormals();
vector<unsigned int> boxIndices = boxMesh.getIndices();

renderer.assignMesh(box, &boxVerts);
renderer.assignNormals(box, &boxNormals);
renderer.assignIndices(box, &boxIndices);
renderer.assignMaterial(box, &tsMat);
i++;
}
}
*/

#endif // GAMEMANAGER_CPP