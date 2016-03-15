#ifndef INPUTSMOOTHER_CPP
#define INPUTSMOOTHER_CPP
#include "InputSmoother.h"


InputSmoother::InputSmoother()
{
	timer = Timer();
	movementTime = timer.getCurrentTime();
}


InputSmoother::~InputSmoother()
{
}

Input InputSmoother::smooth(Input in, bool inAir) {
	Input out = in;

	difference = timer.getTimeSince(movementTime);
	movementTime = timer.getCurrentTime();
	// If 60fps, maxAccelChange = accelStep
	// If less than 60fps, maxAccelChange > accelStep
	// If greater than 60fps, maxAccelChange < accelStep
	float maxAccelChange = accelStep  * (difference / movementCounterEnd);

	// If forward is increasing too much, increase by max instead
	if (out.forward > lastForward && out.forward - lastForward > maxAccelChange) {
		out.forward = lastForward + maxAccelChange;
	}
	// If forward is decreasing too much, decrease by max instead
	else if (out.forward < lastForward && lastForward - out.forward > maxAccelChange) {
		out.forward = lastForward - maxAccelChange;
	}
	// Otherwise, do not change
	lastForward = out.forward;

	// If backward is increasing too much, increase by max instead
	if (out.backward > lastBackward && out.backward - lastBackward > maxAccelChange) {
		out.backward = lastBackward + maxAccelChange;
	}
	// If backward is decreasing too much, decrease by max instead
	else if (out.backward < lastBackward && lastBackward - out.backward > maxAccelChange) {
		out.backward = lastBackward - maxAccelChange;
	}
	// Otherwise, do not change
	lastBackward = out.backward;

	float maxTurnChange = turnStep * (difference / movementCounterEnd);

	// If turnL is increasing too much, increase by max instead
	if (out.turnL > lastTurnL && out.turnL - lastTurnL > maxTurnChange) {
		out.turnL = lastTurnL + maxTurnChange;
	}
	// If turnL is decreasing too much, decrease by max instead
	else if (out.turnL < lastTurnL && lastTurnL - out.turnL > maxTurnChange) {
		out.turnL = lastTurnL - maxTurnChange;
	}
	// Otherwise, do not change
	lastTurnL = out.turnL;

	// If turnR is increasing too much, increase by max instead
	if (out.turnR > lastTurnR && out.turnR - lastTurnR > maxTurnChange) {
		out.turnR = lastTurnR + maxTurnChange;
	}
	// If turnR is decreasing too much, decrease by max instead
	else if (out.turnR < lastTurnR && lastTurnR - out.turnR > maxTurnChange) {
		out.turnR = lastTurnR - maxTurnChange;
	}
	// Otherwise, do not change
	lastTurnR = out.turnR;

	if (inAir) {
		float maxInAirChange = inAirControlStep * (difference / movementCounterEnd);

		if (out.tiltForward > lastTiltForward && out.tiltForward - lastTiltForward > maxInAirChange) {
			out.tiltForward = lastTiltForward + maxInAirChange;
		}
		else if (out.tiltForward < lastTiltForward && lastTiltForward - out.tiltForward > maxInAirChange) {
			out.tiltForward = lastTiltForward - maxInAirChange;
		}
		lastTiltForward = out.tiltForward;

		if (out.tiltBackward > lastTiltBackward && out.tiltBackward - lastTiltBackward > maxInAirChange) {
			out.tiltBackward = lastTiltBackward + maxInAirChange;
		}
		else if (out.tiltBackward < lastTiltBackward && lastTiltBackward - out.tiltBackward > maxInAirChange) {
			out.tiltBackward = lastTiltBackward - maxInAirChange;
		}
		lastTiltBackward = out.tiltBackward;

		if (out.rollL > lastRollL && out.rollL - lastRollL > maxInAirChange) {
			out.rollL = lastRollL + maxInAirChange;
		}
		else if (out.rollL < lastRollL && lastRollL - out.rollL > maxInAirChange) {
			out.rollL = lastRollL - maxInAirChange;
		}
		lastRollL = out.rollL;

		if (out.rollR > lastRollR && out.rollR - lastRollR > maxInAirChange) {
			out.rollR = lastRollR + maxInAirChange;
		}
		else if (out.rollR < lastRollR && lastRollR - out.rollR > maxInAirChange) {
			out.rollR = lastRollR - maxInAirChange;
		}
		lastRollR = out.rollR;

		if (out.spinL > lastSpinL && out.spinL - lastSpinL > maxInAirChange) {
			out.spinL = lastSpinL + maxInAirChange;
		}
		else if (out.spinL < lastSpinL && lastSpinL - out.spinL > maxInAirChange) {
			out.spinL = lastSpinL - maxInAirChange;
		}
		lastSpinL = out.spinL;

		if (out.spinR > lastSpinR && out.spinR - lastSpinR > maxInAirChange) {
			out.spinR = lastSpinR + maxInAirChange;
		}
		else if (out.spinR < lastSpinR && lastSpinR - out.spinR > maxInAirChange) {
			out.spinR = lastSpinR - maxInAirChange;
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

 
	

	// Menu can only be "pressed" once every 5/60ths of a second to prevent
	// effects triggering on button pressed happening too often
	if (out.menu && !menuLock) {
		menuLock = true;
		menuTime = timer.getCurrentTime();
	}
	else if (menuLock && timer.getTimeSince(menuTime) < menuCounterMax) {
		out.menu = false;
	}
	else if (menuLock && timer.getTimeSince(menuTime) >= menuCounterMax) {
		out.menu = false;
		menuLock = false;
	}

	if (out.powerup && !powerupLock) {
		powerupLock = true;
		powerupTime = timer.getCurrentTime();
	}
	else if (powerupLock && timer.getTimeSince(powerupTime) < powerupCounterMax) {
		out.powerup = false;
	}
	else if (powerupLock && timer.getTimeSince(powerupTime) >= powerupCounterMax) {
		out.powerup = false;
		powerupLock = false;
	}

	
	if (out.jump && !jumpLock) {
		jumpLock = true;
		jumpTime = timer.getCurrentTime();
	}
	else if (jumpLock && timer.getTimeSince(jumpTime) < jumpCounterMax) {
		out.jump = false;
	}
	else if (jumpLock && timer.getTimeSince(jumpTime) >= jumpCounterMax) {
		out.jump = false;
		jumpLock = false;
	}
	

	return out;
}

#endif
