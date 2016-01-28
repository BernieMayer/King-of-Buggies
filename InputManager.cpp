#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "InputManager.h"
#include "Gamepad.h"

float kForward;
float kTurn;
float lastMouseX;
float kCamH;
float lastMouseY;
float kCamV;
bool kPowerup;
bool kDrift;
bool kMenu;

void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		kForward += 1;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
		kForward -= 1;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		kForward -= 1;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
		kForward += 1;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		kTurn -= 1;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
		kTurn += 1;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		kTurn += 1;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
		kTurn -= 1;
	}
}

InputManager::InputManager(GLFWwindow* w)
{
	window = w;

	glfwSetKeyCallback(window, keyboard);
	kForward = 0;
	kTurn = 0;
	kCamH = 0;
	kCamV = 0;
	kDrift = false;
	kPowerup = false;
	kMenu = false;

	numPlayers = 1;

	for (int i = 1; i < 5; i++) {
		Gamepad temp = Gamepad(i);

		gamepads[i] = temp;
		if (temp.Connected()) {
			// Allows for a player when no controllers connected
			if (numPlayers != 1) {
				numPlayers += 1;
			}
		}
	}
}


InputManager::~InputManager()
{
}

// Updates and returns input for all players
Input InputManager::getInput(int playerNum)
{
	Input input = Input();

	gamepads[playerNum].Update();

	if (gamepads[playerNum].Connected()) {
		input.forward = gamepads[playerNum].RightTrigger();
		input.forward -= gamepads[playerNum].LeftTrigger();

		if (gamepads[playerNum].LStick_InDeadzone()) {
			input.turn = 0;
		} else {
			input.turn = gamepads[playerNum].LeftStick_X();
		}

		if (gamepads[playerNum].RStick_InDeadzone()) {
			input.camH = 0;
			input.camV = 0;
		} else {
			input.camH = gamepads[playerNum].RightStick_X();
			input.camV = gamepads[playerNum].RightStick_Y();
		}

		// Using XINPUT_GAMEPAD_RIGHT_SHOULDER activates when start is pressed for some reason...
		input.drift = gamepads[playerNum].GetButtonPressed(9);
		// Using XINPUT_GAMEPAD_B never activates for some reason...
		input.powerup = gamepads[playerNum].GetButtonPressed(1);
		
		// Same problem as with B button...
		input.menu = gamepads[playerNum].GetButtonPressed(12);

		gamepads[playerNum].RefreshState();
	} else if (playerNum == 1) {
		// use Keyboard
		input.forward = kForward;
		input.turn = kTurn;
		input.camH = kCamH;
		input.camV = kCamV;
		input.drift = kDrift;
		input.menu = kMenu;
	} else {
		input.forward = 0;
		input.turn = 0;
		input.camH = 0;
		input.camV = 0;
		input.drift = false;
		input.powerup = false;
		input.menu = false;
	}


	return input;
}

// Returns the number of players
int InputManager::getNumPlayers() {
	return numPlayers;
}

