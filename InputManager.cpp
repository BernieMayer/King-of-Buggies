#ifndef INPUTMANAGER_CPP
#define INPUTMANAGER_CPP

#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>

#include "InputManager.h"

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
bool kJump;
bool kHorn;

//Only to easily cheat in coins
bool kCheat_coin;

/**
 * Takes a value between two numbers and converts it to another value between two different numbers
 * maintaining its proportional place between the first two numbers.
 */
float map(float value, float min, float max, float newMin, float newMax) {
	return (value - min) * ((newMax - newMin) / (max - min)) + newMin;
}

/**
 * Called on mouse movement
 * Uses the current mouse position and the mouse position from the last frame and 
 * calculates how much the camera should be rotated based on their differences
 * Rotational values are between -1 and 1
 */
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

/**
 * Called on mouse click
 * Sets boolean values for powerup and drift based on mouse buttons being pressed
 * May need to change to be frame based later...
 */
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

/**
 * Called on keyboard input
 * Increments or decrements floats based on keys being pressed
 */
void keyboard(GLFWwindow *sender, int key, int scancode, int action, int mods) {
	if (forwardKeyboard(key) && action == GLFW_PRESS) {
		kForward += 1;
	}
	else if (forwardKeyboard(key) && action == GLFW_RELEASE) {
		kForward -= 1;
	}
	else if (backwardKeyboard(key) && action == GLFW_PRESS) {
		kBackward += 1;
	}
	else if (backwardKeyboard(key) && action == GLFW_RELEASE) {
		kBackward -= 1;
	}
	else if (leftKeyboard(key) && action == GLFW_PRESS) {
		kTurnL -= 1;
	}
	else if (leftKeyboard(key) && action == GLFW_RELEASE) {
		kTurnL += 1;
	}
	else if (rightKeyboard(key) && action == GLFW_PRESS) {
		kTurnR -= 1;
	}
	else if (rightKeyboard(key) && action == GLFW_RELEASE) {
		kTurnR += 1;
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		kMenu = true;
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		kMenu = false;
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
		// Temporary way to close window
		glfwSetWindowShouldClose(sender, GL_TRUE);
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS && !kCheat_coin){
		if (!kCheat_coin){
			kCheat_coin = true;
		}
	}
	else if (key == GLFW_KEY_C && action == GLFW_RELEASE){
		kCheat_coin = false;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		kJump = true;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		kJump = false;
	}
	else if (hornKeyboard(key) && action == GLFW_PRESS) {
		kHorn = true;
	}
	else if (hornKeyboard(key) && action == GLFW_RELEASE) {
		kHorn = false;
	}
}

/**
 * Creates an instance of InputManager
 * Counts the number of players based on # of conencted controllers
 * at creation
 * No numPlayers is not updated after creation, but it may be possible to
 * simply connect another controller and have it work
 * Always at least 1 player because of mouse and keyboard support
 */
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
	kJump = false;
	kHorn = false;

	rumbleTimer = Timer();

	int widthP = 0;
	int heightP = 0;
	glfwGetWindowSize(window, &widthP, &heightP);

	widthP = widthP / 2;
	heightP = heightP / 2;

	glfwSetCursorPos(window, widthP, heightP);

	lastMouseX = (float)widthP;
	lastMouseY = (float)heightP;

	numPlayers = 1;

	for (int i = 1; i < 5; i++) {
		Gamepad temp = Gamepad(i);

		gamepads[i - 1] = temp;

		if (temp.Connected()) {
			printf("Controller connected in port %d\n", i);
			// Allows for a player when no controllers connected
			if (i != 1) {
				numPlayers += 1;
			}
			rumbleEndTimes[i - 1] = 0;

			//rumble(i, 0.5f, 0.5);
		}
	}
}

/**
 * Deconstructor
 * Maybe needs to do something?
 */
InputManager::~InputManager()
{

}

/**
 * Returns the input of the given player in a Input structure.
 * Input returned can be handled the same no matter if it was 
 * given by controller or keyboard
 * If the player is not connected input for no actions are returned
 */
Input InputManager::getInput(int playerNum)
{
	int smootherNum = playerNum;
	playerNum -= 1;

	Input input = Input();

	gamepads[playerNum].Update();

	if (gamepads[playerNum].Connected()) {
		input.forward = gamepads[playerNum].RightTrigger();
		input.backward = gamepads[playerNum].LeftTrigger();

		if (gamepads[playerNum].LStick_InDeadzone()) {
			input.turnL = 0;
			input.turnR = 0;
			input.tiltForward = 0;
			input.tiltBackward = 0;
			input.rollL = 0;
			input.rollR = 0;
			input.spinL = 0;
			input.spinR = 0;
		} else {
			float turn = gamepads[playerNum].LeftStick_X();
			if (turn <= 0) {
				input.turnL = turn;
				input.turnR = 0;
				if (gamepads[playerNum].GetButtonPressed(driftButton)) {
					input.rollL = turn;
				}
				else {
					input.spinL = turn;
				}
			}
			else {
				input.turnL = 0;
				input.turnR = -turn;
				if (gamepads[playerNum].GetButtonPressed(driftButton)) {
					input.rollR = -turn;
				}
				else {
					input.spinR = -turn;
				}
			}

			float tilt = gamepads[playerNum].LeftStick_Y();
			if (tilt <= 0) {
				input.tiltBackward = -tilt;
				input.tiltForward = 0;
			}
			else {
				input.tiltForward = tilt;
				input.tiltBackward = 0;
			}
		}

		if (gamepads[playerNum].RStick_InDeadzone()) {
			input.camH = 0;
			input.camV = 0;
		} else {
			input.camH = gamepads[playerNum].RightStick_X();
			input.camV = gamepads[playerNum].RightStick_Y();
		}

		input.drift = gamepads[playerNum].GetButtonPressed(driftButton);
		input.powerup = gamepads[playerNum].GetButtonPressed(powerupButton);
		input.menu = gamepads[playerNum].GetButtonPressed(menuButton);
		input.jump = gamepads[playerNum].GetButtonPressed(jumpButton);
		input.horn = gamepads[playerNum].GetButtonPressed(honkButton);

		gamepads[playerNum].RefreshState();

		input.isKeyboard = false;

	
		if (rumbleEndTimes[playerNum] > 0) {
			if (rumbleTimer.getTimeSince(rumbleStartTimes[playerNum]) >= rumbleEndTimes[playerNum]) {
				gamepads[playerNum].Rumble(0.0f, 0.0f);
				rumbleEndTimes[playerNum] = 0;
			}
		}
	} else if (playerNum == 0) {
		// use Keyboard
		input.forward = kForward;
		input.backward = kBackward;
		input.turnL = kTurnL;
		input.turnR = kTurnR;
		input.tiltForward = kForward;
		input.tiltBackward = kBackward;
		input.camH = kCamH;
		input.camV = kCamV;
		input.drift = kDrift;
		input.powerup = kPowerup;
		input.menu = kMenu;
		input.jump = kJump;
		input.horn = kHorn;

		if (kDrift) {
			input.rollL = kTurnL;
			input.rollR = kTurnR;
			input.spinL = 0;
			input.spinR = 0;
		}
		else {
			input.rollL = 0;
			input.rollR = 0;
			input.spinL = kTurnL;
			input.spinR = kTurnR;
		}
		
		input.isKeyboard = true;

		//only to easily activate coins
		input.cheat_coin = kCheat_coin;

		int widthP = 0;
		int heightP = 0;
		glfwGetWindowSize(window, &widthP, &heightP);

		widthP = widthP / 2;
		heightP = heightP / 2;

		glfwSetCursorPos(window, widthP, heightP);
		
		lastMouseX = (float)widthP;
		lastMouseY = (float)heightP;

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
		input.jump = false;
		input.horn = false;
		input.isKeyboard = false;
	}


	return input;
}

/**
 * Returns the number of players
 */
int InputManager::getNumPlayers() {
	return numPlayers;
}


void InputManager::rumble(int playerNum, float strength, float timeInSec) {
	if (!gamepads[playerNum - 1].Connected()) {
		return;
	}
	gamepads[playerNum - 1].Rumble(strength, 0.0f);

	rumbleStartTimes[playerNum - 1] = rumbleTimer.getCurrentTime();
	rumbleEndTimes[playerNum - 1] = timeInSec;
}

#endif
