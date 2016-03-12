#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Gamepad.h"
#include "Keybindings.h"
#include "InputSmoother.h"

class InputManager
{
public:
	InputManager(GLFWwindow* window);
	~InputManager();
	// Get input for the given player
	Input getInput(int playerNum);
	// Returns the number of players
	int getNumPlayers();
	void rumble(int playerNum, float strength, int timeFrames);
private:
	const int driftButton = 2;
	const int powerupButton = 1;
	const int menuButton = 12;
	const int jumpButton = 0;
	const int honkButton = 3;

	GLFWwindow* window;
	Gamepad gamepads[4];
	int numPlayers;
	int rumbleCounters[4];
	int rumbleTargets[4];
	
};

#endif //INPUTMANAGER_H