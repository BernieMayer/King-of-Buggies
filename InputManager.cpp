#include "InputManager.h"
#include "Gamepad.h"


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}
// Updates and returns input for all players
Input* InputManager::getInput()
{
	return 0;
}
// Used to notify game manager of the amount of forward/backward acceleration 
// the player is requesting
void InputManager::notifyForward(float accel)
{
}
// Used to notify game manager of the amount of turning the player has requested
void InputManager::notifyTurn(float accel)
{
}
// Used to notify game manager that the player has requested to start a drift
void InputManager::notifyDriftStart()
{
}
// Used to notify game manager that the player has requested to end a drift
void InputManager::notifyDriftEnd()
{
}
// Used to notify game manger that the player has requested to use a power-up
void InputManager::notifyPowerup()
{
}

// Used to notify game manager that the player has requested to tilt camera up/down
void InputManager::notifyTiltCameraV(float accel)
{
}
// Used to notify game manager that the player has requested to tilt camera left/right
void InputManager::notifyTiltCameraH(float accel)
{
}
// Used to notify game manager that the plaayer has pressed the menu button
void InputManager::menuButtonPressed()
{
}


