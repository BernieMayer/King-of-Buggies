#ifndef INPUTSMOOTHER_H
#define INPUTSMOOTHER_H

#include "Input.h"
#include "Timer.h"
#include <iostream>

using namespace std;

class InputSmoother
{
public:
	InputSmoother();
	~InputSmoother();
	Input smooth(Input in, bool inAir);

	float difference;

private:
	float lastForward = 0.f;
	float lastBackward = 0.f;
	float lastTurnL = 0.f;
	float lastTurnR = 0.f;
	float lastSpinL = 0.f;
	float lastSpinR = 0.f;
	float lastRollL = 0.f;
	float lastRollR = 0.f;
	float lastTiltForward = 0.f;
	float lastTiltBackward = 0.f;
	// Set higher to have faster changes
	// Lower to have smoother changes
	float accelStep = 0.08f;
	float turnStep = 0.08f;
	float inAirControlStep = 0.3f;

	Timer timer;
	timeb movementTime;
	// 1/60th of a second since this seemed to work niceley at 60 frames a second
	float movementCounterEnd = 1.0f / 60.0f;

	bool menuLock = false;
	timeb menuTime;
	// 5/60th of a second since 5 frames seemed to work nicely
	float menuCounterMax = 5.0f / 60.0f;

	bool powerupLock = false;
	timeb powerupTime;
	float powerupCounterMax = 5.0f / 60.0f;

	bool jumpLock = false;
	timeb jumpTime;
	float jumpCounterMax = 5.0f / 60.0f;
};

#endif // INPUTSMOOTHER_H

