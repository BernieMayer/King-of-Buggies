#include <GL/glew.h>  
#include <GLFW/glfw3.h>

#include "InputManager.h"
#include "Gamepad.h"

float kForward;
float kBackward;
float kTurnL;
float kTurnR;
float lastMouseX;
float currentMouseX;
float kCamH;
float lastMouseY;
float currentMouseY;
float kCamV;
bool kPowerup;
bool kDrift;
bool kMenu;

float map(float value, float min, float max, float newMin, float newMax) {
	return (value - min) * ((newMax - newMin) / (max - min)) + newMin;
}

// Handles mouse movement
void mousePosition(GLFWwindow *sender, double x, double y) {

	float f_x = (float)x;
	float f_y = (float)y;

	float deltaX = f_x - lastMouseX;
	float maxDeltaX = 50;
	float minDeltaX = -50;
	if (deltaX > maxDeltaX) {
		deltaX = maxDeltaX;
	}
	else if (deltaX < minDeltaX) {
		deltaX = minDeltaX;
	}
	kCamH = map(deltaX, minDeltaX, maxDeltaX, -1.0, 1.0);
	currentMouseX = f_x;

	float deltaY = f_y - lastMouseY;
	float maxDeltaY = 50;
	float minDeltaY = -50;
	if (deltaY > maxDeltaY) {
		deltaY = maxDeltaY;
	}
	else if (deltaY < minDeltaY) {
		deltaY = minDeltaY;
	}
	kCamV = map(deltaY, minDeltaY, maxDeltaY, -1.0, 1.0);
	currentMouseY = f_y;
}

// Handles mouse button input
void mouseClick(GLFWwindow *sender, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		kPowerup = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		kPowerup = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		kDrift = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		kDrift = false;
	}
}

// Handles keyboard input
void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		kForward += 1;
	}
	else if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
		kForward -= 1;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		kBackward += 1;
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
		kBackward -= 1;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		kTurnL += 1;
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) {
		kTurnL -= 1;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		kTurnR += 1;
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) {
		kTurnR -= 1;
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		kMenu = true;
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		kMenu = false;
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		// Temporary way to close window
		glfwDestroyWindow(sender);
		glfwTerminate();
	}
}

InputManager::InputManager(GLFWwindow* w)
{
	window = w;

	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, mouseClick);
	glfwSetCursorPosCallback(window, mousePosition);
	kForward = kBackward = 0;
	kTurnL = kTurnR = 0;
	kCamH = 0;
	kCamV = 0;
	kDrift = false;
	kPowerup = false;
	kMenu = false;

	numPlayers = 1;

	for (int i = 1; i < 5; i++) {
		Gamepad temp = Gamepad(i);

		gamepads[i - 1] = temp;

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
	playerNum -= 1;

	Input input = Input();

	gamepads[playerNum].Update();

	if (gamepads[playerNum].Connected()) {
		input.forward = gamepads[playerNum].RightTrigger();
		input.backward = gamepads[playerNum].LeftTrigger();

		if (gamepads[playerNum].LStick_InDeadzone()) {
			input.turnL = 0;
			input.turnR = 0;
		} else {
			float turn = gamepads[playerNum].LeftStick_X();
			if (turn <= 0) {
				input.turnL = turn;
				input.turnR = 0;
			}
			else {
				input.turnL = 0;
				input.turnR = turn;
			}
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

		input.isKeyboard = false;
	} else if (playerNum == 0) {
		// use Keyboard
		input.forward = kForward;
		input.backward = kBackward;
		input.turnL = kTurnL;
		input.turnR = kTurnR;
		input.camH = kCamH;
		input.camV = kCamV;
		input.drift = kDrift;
		input.powerup = kPowerup;
		input.menu = kMenu;
		input.isKeyboard = true;


		int widthP = 0;
		int heightP = 0;
		glfwGetWindowSize(window, &widthP, &heightP);

		widthP = widthP / 2;
		heightP = heightP / 2;

		glfwSetCursorPos(window, widthP, heightP);
		
		lastMouseX = widthP;
		lastMouseY = heightP;

		kCamH = kCamV = 0.f;
	} else {
		input.forward = 0;
		input.backward = 0;
		input.turnL = 0;
		input.turnR = 0;
		input.camH = 0;
		input.camV = 0;
		input.drift = false;
		input.powerup = false;
		input.menu = false;
		input.isKeyboard = false;
	}


	return input;
}

// Returns the number of players
int InputManager::getNumPlayers() {
	return numPlayers;
}

