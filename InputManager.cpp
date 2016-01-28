#include "InputManager.h"
#include "Gamepad.h"

InputManager::InputManager()
{
	numPlayers = 1;

	for (int i = 1; i < 5; i++) {
		Gamepad *temp = new Gamepad(i);

		if (temp->Connected()) {
			gamepads[i] = *temp;
			// Allows for a player when no controllers connected
			if (numPlayers != 1) {
				numPlayers += 1;
			}
		}
		else {
			gamepads[i] = NULL;
		}
	}
}


InputManager::~InputManager()
{
}

// Updates and returns input for all players
Input InputManager::getInput(int playerNum)
{
	Input *input = new Input();

	input->forward = gamepads[playerNum].RightTrigger();

	return *input;
}

// Returns the number of players
int InputManager::getNumPlayers() {
	return numPlayers;
}

