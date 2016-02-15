#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AIManager.h"
#include "DrivingController.h"
#include "GameState.h"
#include "InputManager.h"
#include "InterfaceManager.h"
#include "MeshInfo.h"
#include "Physics.h"
#include "Renderer.h"
#include "SoundManager.h"

#include <GLFW/glfw3.h>  

class GameManager {
private:
	// Object materials
	Diffuse mat;
	Specular shinyMat;
	TorranceSparrow tsMat;

	AIManager ai;
	DrivingController dc;
	GameState state;
	InputManager input;
	InterfaceManager _interface;
	MeshInfo meshInfo;
	Physics physics;
	Renderer renderer;
	SoundManager sound;

	GLFWwindow* window;

public:
	GameManager(GLFWwindow* newWindow);

	// create physics and render objects through GameState
	void createPlayer(vec3 position);		//Add forward parameter later, maybe also VehicleTraits
	void createCoins();
	void createPowerups();
	void createPowerupBoxes();
	
	void gameLoop();
	void gameInit();
	void initTestScene();
	void quitGame(); // should this take arguments? 

	void physicsAndRenderTest();

};

#endif // GAMEMANAGER_H