#ifndef GAMEMANAGER_CPP
#define GAMEMANAGER_CPP

#include "GameManager.h"

void GameManager::physicsAndRenderTest()
{
	printf("Begin physics and rendering test\n");

	//Object creation
	//Create plane
	unsigned int plane = renderer.generateObjectID();
	vector<vec3> meshPlane;
	vector<vec3> normalsPlane;
	vector<unsigned int> indicesPlane;
	renderer.assignPlane(plane, 8.f, &meshPlane, &normalsPlane, &indicesPlane);
	renderer.assignMaterial(plane, &tsMat);
	renderer.assignColor(plane, vec3(0.5f, 0.5f, 0.5f));

	//Create sphere
	vector<vec3> mesh;
	vector<vec3> normals;
	vector<unsigned int> indices;

	unsigned int sphere = renderer.generateObjectID();
	renderer.assignSphere(sphere, 0.5f, 20, &mesh, &normals, &indices);
	renderer.assignMaterial(sphere, &tsMat);

	MeshObject carMesh = meshInfo.getMesh(CUBE);
	vector<vec3> carVerts = carMesh.getVertices();
	vector<vec3> carNormals = carMesh.getNormals();
	vector<unsigned int> carIndices = carMesh.getIndices();

	unsigned int car = renderer.generateObjectID();
	renderer.assignMesh(car, &carVerts);
	renderer.assignNormals(car, &carNormals);
	renderer.assignIndices(car, &carIndices);
	renderer.assignMaterial(car, &tsMat);

	MeshObject wheelMesh = meshInfo.getMesh(WHEEL);
	vector<vec3> wheelVerts = wheelMesh.getVertices();
	vector<vec3> wheelNormals = wheelMesh.getNormals();
	vector<unsigned int> wheelIndices = wheelMesh.getIndices();

	unsigned int wheels[4];
	mat4 wheelScaling(1.f);
	wheelScaling[0][0] = 0.4f;
	wheelScaling[1][1] = 0.5f;
	wheelScaling[2][2] = 0.5f;

	for (int i = 0; i < 4; i++)
	{
		wheels[i] = renderer.generateObjectID();
		renderer.assignMesh(wheels[i], &wheelVerts);
		renderer.assignNormals(wheels[i], &wheelNormals);
		renderer.assignIndices(wheels[i], &wheelIndices);
		renderer.assignMaterial(wheels[i], &tsMat);
		renderer.assignScale(wheels[i], wheelScaling);
	}


	printf("Vertices = %d, Normals %d, Indices %d\n", (carMesh.getVertices()).size()
		, (carMesh.getNormals()).size(), (carMesh.getIndices()).size());


	//Light creation
	vec3 lightPos(5.0, 5.0, 5.0);
	unsigned int light = renderer.generateLightObject();
	renderer.setLightPosition(light, lightPos);


	//Starting direction, up vector, starting position
	Camera cam(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 5.0),
		MODELVIEWER_CAMERA);

	renderer.loadPerspectiveTransform(0.1f, 20.f, 90.f);		//Near, far, fov

	renderer.loadCamera(&cam);

	mat4 translation;
	float theta = 0.0;

	mat4 carScaling(1.f);
	carScaling[0][0] = 1.25f*0.5f;
	carScaling[1][1] = 1.f*0.5f;
	carScaling[2][2] = 2.f*0.5f;
	renderer.assignScale(car, carScaling);

	//cam.changeCenterAndPos(vec3(0.5f, 0.5f, 0.5f));


	while (!glfwWindowShouldClose(window))
	{

		Input in = input.getInput(1);		//Get input
		physics.handleInput(&in);

		//Physics sim
		physics.startSim(GameState());
		physics.getSim();

		float scale = 0.1f;

		if (!in.drift)//in.powerup)
			cam.rotateView(in.camH*scale, in.camV*scale);
		if (in.drift)
			cam.zoom(in.camV*0.95f + 1.f);

		physx::PxRigidActor* sphereActor = getSphere();
		physx::PxRigidDynamic* carActor = getCar();

		renderer.assignTransform(sphere, getMat4(sphereActor->getGlobalPose()));
		renderer.assignTransform(car, getMat4(carActor->getGlobalPose()));

		vec4 cameraPosition = getMat4(carActor->getGlobalPose())*vec4(0.f, 0.f, 0.f, 1.f);
		vec3 cPos = vec3(cameraPosition) / cameraPosition.w;

		if (cPos != cam.getViewCenter())
			cam.changeCenterAndPos(cPos - cam.getViewCenter());

		//Track camera around front of vehicle
		vec4 carDir = getMat4(carActor->getGlobalPose())*vec4(0.f, 0.f, 1.f, 0.f);
		
		//if (in.forward > 0.f)
			cam.trackDirAroundY(vec3(carDir), 1.f / 60.f);

		//Wheel transformations
		PxShape* wheelShapes[4];
		getWheels(wheelShapes);
		for (int i = 0; i < 4; i++)
		{
			renderer.assignTransform(wheels[i], getMat4(carActor->getGlobalPose())*getMat4(wheelShapes[i]->getLocalPose()));
		}

		renderer.clearDrawBuffers();

		renderer.drawAll();

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("End rendering test\n");
}

GameManager::GameManager(GLFWwindow* newWindow) : renderer(newWindow), input(newWindow), state(), physics()
{
	window = newWindow;
	mat = Diffuse();
	shinyMat = Specular(20.f);
	tsMat = TorranceSparrow(3.f);
}
/*
void GameManager::createPlayers()
{
	int i = 0;
	while (i < NUM_PLAYERS) {
		unsigned int player = renderer.generateObjectID();
		state.getPlayer(i)->setRenderID(player);
		
		MeshObject playerMesh = meshInfo.getMesh(BUGGY);
		vector<vec3> playerVerts = playerMesh.getVertices();
		vector<vec3> playerNormals = playerMesh.getNormals();
		vector<unsigned int> playerIndices = playerMesh.getIndices();

		renderer.assignMesh(player, &playerVerts);
		renderer.assignNormals(player, &playerNormals);
		renderer.assignIndices(player, &playerIndices);
		renderer.assignMaterial(player, &tsMat);
		i++;
	}
}

void GameManager::createCoins()
{
	int i = 0;
	while (i < NUM_COINS) {
		unsigned int coin = renderer.generateObjectID();
		state.getCoin(i)->setRenderID(coin);

		// leave commented for now, there's no coin mesh
		//MeshObject coinMesh = meshInfo.getMesh(COIN);
		MeshObject coinMesh = meshInfo.getMesh(SPHERE);
		vector<vec3> coinVerts = coinMesh.getVertices();
		vector<vec3> coinNormals = coinMesh.getNormals();
		vector<unsigned int> coinIndices = coinMesh.getIndices();

		renderer.assignMesh(coin, &coinVerts);
		renderer.assignNormals(coin, &coinNormals);
		renderer.assignIndices(coin, &coinIndices);
		renderer.assignMaterial(coin, &tsMat);
		i++;
	}
}

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
void GameManager::gameLoop()
{
	physicsAndRenderTest();
}

void GameManager::gameInit()
{

}

void quitGame()
{

}

#endif // GAMEMANAGER_CPP