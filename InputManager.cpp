#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "InputManager.h"
#include "Gamepad.h"

void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods) {
		
}

InputManager::InputManager(GLFWwindow* w)
{
	window = w;

	glfwSetKeyCallback(window, keyboard);

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

