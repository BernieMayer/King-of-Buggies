#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "Input.h"
#include "Gamepad.h"

class InputManager
{
public:
	InputManager(GLFWwindow* window);
	~InputManager();
	// Get input for the given player
	Input getInput(int playerNum);
	// Returns the number of players
	int getNumPlayers();
private:
	GLFWwindow* window;
	Gamepad gamepads[4];
	int numPlayers;

	
};

#endif //INPUTMANAGER_H