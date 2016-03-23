#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Gamepad.h"
#include "Keybindings.h"
#include "InputSmoother.h"
#include "Timer.h"

class InputManager
{
public:
	InputManager(GLFWwindow* window);
	~InputManager();
	// Get input for the given player
	Input getInput(int playerNum);
	// Returns the number of players
	int getNumPlayers();
	void rumble(int playerNum, float strength, float timeInSec);
private:
	const int driftButton = 2;
	const int powerupButton = 1;
	const int menuButton = 12;
	const int jumpButton = 0;
	const int honkButton = 3;

	GLFWwindow* window;
	Gamepad gamepads[5];
	int numPlayers;
	Timer rumbleTimer;
	timeb rumbleStartTimes[5];
	float rumbleEndTimes[5];
	
};

#endif //INPUTMANAGER_H