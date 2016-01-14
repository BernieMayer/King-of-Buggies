#include "Gamepad.h"
#include <iostream>


// A quick example of how to use this
int main() {

	Gamepad* controller;
	controller = new Gamepad(1);

	float LX = 0;
	float LY = 0;

	while (true) {
		controller->Update();

		if (controller->GetButtonPressed(XButtons.A)) {
			std::cout << "A\n";
		}

		if (!(controller->LStick_InDeadzone()) &&
			(controller->LeftStick_X() != LX ||
				controller->LeftStick_Y() != LY))
		{
			LX = controller->LeftStick_X();
			LY = controller->LeftStick_Y();
			std::cout << "X: " << LX << '\n';
			std::cout << "Y: " << LY << '\n';
			std::cout << '\n';
		}

		controller->RefreshState();
	}

	return 0;
}