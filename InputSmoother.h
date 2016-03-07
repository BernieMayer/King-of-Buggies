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
	float accelStep = 0.08f;
	float turnStep = 0.08f;

	bool menuLock = false;
	int menuFrameCounter = 0;
	int menuCounterMax = 5;

	bool powerupLock = false;
	int powerupFrameCounter = 0;
	int powerupCounterMax = 5;

	bool jumpLock = false;
	int jumpFrameCounter = 0;
	int jumpCounterMax = 5;
};

#endif // INPUTSMOOTHER_H

