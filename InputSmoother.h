#ifndef INPUTSMOOTHER_H
#define INPUTSMOOTHER_H

#include "Input.h"

class InputSmoother
{
public:
	InputSmoother();
	~InputSmoother();
	Input smooth(Input in);

private:
	float lastForward = 0;
	float lastBackward = 0;
	float lastTurnL = 0;
	float lastTurnR = 0;
	// Set higher to have faster changes
	// Lower to have smoother changes
	float accelStep = 0.01;
	float turnStep = 0.05;

	bool menuLock = false;
	float menuFrameCounter = 0;

	bool powerupLock = false;
	float powerupFrameCounter = 0;
};

#endif // INPUTSMOOTHER_H

