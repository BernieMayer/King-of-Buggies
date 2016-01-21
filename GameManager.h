#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Renderer.h"
#include "AIManager.h"
#include "DrivingController.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "InterfaceManager.h"

class GameManager {
public:
	void gameLoop();
	void gameInit();
	void quitGame(); // should this take arguments? 

private:
	Renderer renderer;
	AIManager ai;
	DrivingController dc;
	InputManager input;
	SoundManager sound;
	InterfaceManager interface;
};

#endif // GAMEMANAGER_H