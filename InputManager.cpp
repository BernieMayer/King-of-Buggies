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

	gamepads[playerNum].Update();

	if (gamepads[playerNum].Connected()) {
		input->forward = gamepads[playerNum].RightTrigger();
		input->forward -= gamepads[playerNum].LeftTrigger();

		if (gamepads[playerNum].LStick_InDeadzone()) {
			input->turn = 0;
		} else {
			input->turn = gamepads[playerNum].LeftStick_X();
		}

		if (gamepads[playerNum].RStick_InDeadzone()) {
			input->camH = 0;
			input->camV = 0;
		} else {
			input->camH = gamepads[playerNum].RightStick_X();
			input->camV = gamepads[playerNum].RightStick_Y();
		}

		input->drift = gamepads[playerNum].GetButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER);
		input->powerup = gamepads[playerNum].GetButtonPressed(XINPUT_GAMEPAD_B);
		
		input->menu = gamepads[playerNum].GetButtonDown(XINPUT_GAMEPAD_START);

		gamepads[playerNum].RefreshState();
	} else if (playerNum == 1) {
		// use Keyboard
	} else {
		input->forward = 0;
		input->turn = 0;
		input->camH = 0;
		input->camV = 0;
		input->drift = false;
		input->powerup = false;
		input->menu = false;
	}


	return *input;
}

// Returns the number of players
int InputManager::getNumPlayers() {
	return numPlayers;
}

