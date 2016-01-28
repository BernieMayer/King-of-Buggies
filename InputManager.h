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
	//void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods);
	// Get input for the given player
	Input getInput(int playerNum);
	// Returns the number of players
	int getNumPlayers();
private:
	GLFWwindow* window;
	Gamepad gamepads[4];
	int numPlayers;

	//float kForward;
	//float kTurn;
	//float lastMouseX;
	//float kCamH;
	//float lastMouseY;
	//float kCamV;
	//bool kPowerup;
	//bool kDrift;
	//bool kMenu;
};

#endif //INPUTMANAGER_H