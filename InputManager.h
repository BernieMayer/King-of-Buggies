#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "Input.h"
#include "Gamepad.h"

class InputManager
{
public:
	InputManager();
	~InputManager();
	// Get input for the given player
	Input getInput(int playerNum);
	// Returns the number of players
	int getNumPlayers();
private:
	Gamepad gamepads[4];
	int numPlayers;
};

#endif //INPUTMANAGER_H