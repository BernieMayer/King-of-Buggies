#ifndef INPUTSMOOTHER_CPP
#define INPUTSMOOTHER_CPP
#include "InputSmoother.h"


InputSmoother::InputSmoother()
{
}


InputSmoother::~InputSmoother()
{
}

Input InputSmoother::smooth(Input in, bool inAir) {
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

	if (inAir) {
		if (out.tiltForward > lastTiltForward && out.tiltForward - lastTiltForward > inAirControlStep) {
			out.tiltForward = lastTiltForward + inAirControlStep;
		}
		else if (out.tiltForward < lastTiltForward && lastTiltForward - out.tiltForward > inAirControlStep) {
			out.tiltForward = lastTiltForward - inAirControlStep;
		}
		lastTiltForward = out.tiltForward;

		if (out.tiltBackward > lastTiltBackward && out.tiltBackward - lastTiltBackward > inAirControlStep) {
			out.tiltBackward = lastTiltBackward + inAirControlStep;
		}
		else if (out.tiltBackward < lastTiltBackward && lastTiltBackward - out.tiltBackward > inAirControlStep) {
			out.tiltBackward = lastTiltBackward - inAirControlStep;
		}
		lastTiltBackward = out.tiltBackward;

		if (out.rollL > lastRollL && out.rollL - lastRollL > inAirControlStep) {
			out.rollL = lastRollL + inAirControlStep;
		}
		else if (out.rollL < lastRollL && lastRollL - out.rollL > inAirControlStep) {
			out.rollL = lastRollL - inAirControlStep;
		}
		lastRollL = out.rollL;

		if (out.rollR > lastRollR && out.rollR - lastRollR > inAirControlStep) {
			out.rollR = lastRollR + inAirControlStep;
		}
		else if (out.rollR < lastRollR && lastRollR - out.rollR > inAirControlStep) {
			out.rollR = lastRollR - inAirControlStep;
		}
		lastRollR = out.rollR;

		if (out.spinL > lastSpinL && out.spinL - lastSpinL > inAirControlStep) {
			out.spinL = lastSpinL + inAirControlStep;
		}
		else if (out.spinL < lastSpinL && lastSpinL - out.spinL > inAirControlStep) {
			out.spinL = lastSpinL - inAirControlStep;
		}
		lastSpinL = out.spinL;

		if (out.spinR > lastSpinR && out.spinR - lastSpinR > inAirControlStep) {
			out.spinR = lastSpinR + inAirControlStep;
		}
		else if (out.spinR < lastSpinR && lastSpinR - out.spinR > inAirControlStep) {
			out.spinR = lastSpinR - inAirControlStep;
		}
		lastSpinR = out.spinR;
	}
	else {
		lastTiltForward = 0;
		lastTiltBackward = 0;
		lastRollL = 0;
		lastRollR = 0;
		lastSpinL = 0;
		lastSpinR = 0;
	}

	// Menu can only be "pressed" once every 20 frames to prevent
	// effects triggering on button pressed happening too often
	if (out.menu && !menuLock) {
		menuLock = true;
	}
	else if (menuLock) {
		out.menu = false;
		menuFrameCounter += 1;
	}
	if (menuFrameCounter == menuCounterMax) {
		menuFrameCounter = 0;
		menuLock = false;
	}

	if (out.powerup && !powerupLock) {
		powerupLock = true;
	}
	else if (powerupLock) {
		out.powerup = false;
		powerupFrameCounter += 1;
	}
	if (powerupFrameCounter == powerupCounterMax) {
		powerupFrameCounter = 0;
		powerupLock = false;
	}

	
	if (out.jump && !jumpLock) {
		jumpLock = true;
	}
	else if (jumpLock) {
		out.jump = false;
		jumpFrameCounter += 1;
	}
	if (jumpFrameCounter == jumpCounterMax) {
		jumpFrameCounter = 0;
		jumpLock = false;
	}
	

	return out;
}

#endif
