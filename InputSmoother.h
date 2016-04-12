#ifndef INPUTSMOOTHER_H
#define INPUTSMOOTHER_H

#include "Input.h"
#include "Timer.h"
#include <iostream>
#include <vector>

using namespace std;

class InputSmoother
{
public:
	InputSmoother();
	~InputSmoother();
	Input smooth(Input in, bool inAir);

	float difference;

private:
	vector<Input> lastInputs;
	bool checkKonamiCode();

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

	bool powerupLock = false;
	timeb powerupTime;
	// 10 60ths of a second might be better than 5
	float powerupCounterMax = 10.0f / 60.0f;

	bool jumpLock = false;
	timeb jumpTime;
	float jumpCounterMax = 10.0f / 60.0f;

	bool debugLock = false;
	timeb debugTime;
	float debugCounterMax = 10.0f / 60.0f;

	bool navMeshLock = false;
	timeb navMeshTime;
	float navMeshCounterMax = 10.0f / 60.0f;

	bool cheatLock = false;
	timeb cheatTime;
	float cheatCounterMax = 10.0f / 60.0f;

	bool konamiLock = false;
	timeb konamiTime;
	// 5 seconds before another code can be entered
	float konamiCounterMax = 5.0f;
};

#endif // INPUTSMOOTHER_H

