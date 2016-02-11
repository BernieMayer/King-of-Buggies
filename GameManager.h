#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Renderer.h"
#include "AIManager.h"
#include "DrivingController.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "InterfaceManager.h"
#include "GameState.h"
#include "MeshInfo.h"

class GameManager {
private:
	// Object creation
	Diffuse mat = Diffuse();
	Specular shinyMat = Specular(20.f);
	TorranceSparrow tsMat = TorranceSparrow(3.f);

	Renderer renderer;
	AIManager ai;
	DrivingController dc;
	InputManager input;
	SoundManager sound;
	InterfaceManager _interface;
	GameState state;
	MeshInfo meshInfo;

public:
	GameManager(GLFWwindow* window);

	// create physics and render objects through GameState
	void createPlayers();
	void createCoins();
	void createPowerups();
	void createPowerupBoxes();

	void gameLoop();
	void gameInit();
	void quitGame(); // should this take arguments? 

};

#endif // GAMEMANAGER_H