#ifndef INPUTSMOOTHER_CPP
#define INPUTSMOOTHER_CPP
#include "InputSmoother.h"


InputSmoother::InputSmoother()
{
}


InputSmoother::~InputSmoother()
{
}

Input InputSmoother::smooth(Input in) {
	Input out = in;

	// If forward is increasing too much, increase by max instead
	if (out.forward > lastForward && out.forward - lastForward > accelStep) {
		out.forward = lastForward + accelStep;
	}
	// If forward is decreasing too much, decrease by max instead
	else if (out.forward < lastForward && lastForward - out.forward > accelStep) {
		out.forward = lastForward - accelStep;
	}
	// Otherwise, do not change
	lastForward = out.forward;

	// If backward is increasing too much, increase by max instead
	if (out.backward > lastBackward && out.backward - lastBackward > accelStep) {
		out.backward = lastBackward + accelStep;
	}
	// If backward is decreasing too much, decrease by max instead
	else if (out.backward < lastBackward && lastBackward - out.backward > accelStep) {
		out.backward = lastBackward - accelStep;
	}
	// Otherwise, do not change
	lastBackward = out.backward;

	// If turnL is increasing too much, increase by max instead
	if (out.turnL > lastTurnL && out.turnL - lastTurnL > turnStep) {
		out.turnL = lastTurnL + turnStep;
	}
	// If turnL is decreasing too much, decrease by max instead
	else if (out.turnL < lastTurnL && lastTurnL - out.turnL > turnStep) {
		out.turnL = lastTurnL - turnStep;
	}
	// Otherwise, do not change
	lastTurnL = out.turnL;

	// If turnR is increasing too much, increase by max instead
	if (out.turnR > lastTurnR && out.turnR - lastTurnR > turnStep) {
		out.turnR = lastTurnR + turnStep;
	}
	// If turnR is decreasing too much, decrease by max instead
	else if (out.turnR < lastTurnR && lastTurnR - out.turnR > turnStep) {
		out.turnR = lastTurnR - turnStep;
	}
	// Otherwise, do not change
	lastTurnR = out.turnR;

	if (out.menu && !menuLock) {
		menuLock = true;
	}
	else if (menuLock) {
		out.menu = false;
		menuFrameCounter += 1;
	}
	if (menuFrameCounter == 20) {
		menuFrameCounter = 0;
		menuLock = false;
	}

	return out;
}

#endif
