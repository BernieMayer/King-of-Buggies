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
	// Keeps 3 seconds of input
	if (lastInputs.size() > 180) {
		lastInputs.erase(lastInputs.begin());
	}
	lastInputs.push_back(in);

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
	}
	else if (menuLock && out.menu) {
		out.menu = false;
	}
	else if (menuLock && !out.menu) {
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
	
	if (out.cheat_coin && !cheatLock) {
		cheatLock = true;
		cheatTime = timer.getCurrentTime();
	}
	else if (cheatLock && timer.getTimeSince(cheatTime) < cheatCounterMax) {
		out.cheat_coin = false;
	}
	else if (cheatLock && timer.getTimeSince(cheatTime) >= cheatCounterMax) {
		out.cheat_coin = false;
		cheatLock = false;
	}

	if (out.debug && !debugLock) {
		debugLock = true;
		debugTime = timer.getCurrentTime();
	}
	else if (debugLock && timer.getTimeSince(debugTime) < debugCounterMax) {
		out.debug = false;
	}
	else if (debugLock && timer.getTimeSince(debugTime) >= debugCounterMax) {
		out.debug = false;
		debugLock = false;
	}



	if (konamiLock) {
		if (timer.getTimeSince(konamiTime) >= konamiCounterMax) {
			konamiLock = false;
		}
	}

	if (!konamiLock) {
		out.konamiCode = checkKonamiCode();
		if (out.konamiCode) {
			out.menu = false;
		}
	}

	return out;
}

bool InputSmoother::checkKonamiCode() {
	if (lastInputs.back().menu) {
		// If last part right, continue checking backwards
		int counter = lastInputs.size() - 2;
		int konamiStep = 0;
		while (counter >= 0) {
			Input in = lastInputs[counter];
			if (konamiStep == 0) {
				if (in.drift || in.menu ||
					in.horn || in.powerup) {
					return false;
				}
				else if (in.jump) {
					// A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 1) {
				if (in.drift || in.menu || in.horn) {
					return false;
				}
				else if (in.jump) {
					// Do nothing
					// Allows for a button press of A which lasts for more than 
					// 1 frame
				}
				else if (in.powerup) {
					// B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 2) {
				if (in.drift || in.menu || in.horn || in.jump) {
					return false;
				}
				else if (in.powerup) {
					//Do nothing
				}
				else if (-in.turnR >= 0.9) {
					// right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 3) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (-in.turnR > 0) {
					// Do nothing
				}
				else if (-in.turnL > 0.9) {
					// left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 4) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (-in.turnL > 0) {
					// Do nothing
				}
				else if (-in.turnR > 0.9) {
					// right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 5) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (-in.turnR > 0) {
					// Do nothing
				}
				else if (-in.turnL > 0.9) {
					// left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 6) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltBackward > 0.9) {
					// down, left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 7) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltBackward > 0.4) {
					// Do nothing
				}
				else {
					// neutral, down, left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 8) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltBackward < 0.9) {
					// Do nothing
				}
				else {
					// down, down, left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 9) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltForward < 0.9) {
					// Do nothing
				}
				else {
					// up, down, down, left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 10) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltForward > 0.4) {
					// do nothing
				}
				else {
					// neutral, up, down, down, left, right, left, right, B, A, start
					konamiStep++;
				}
			}
			else if (konamiStep == 11) {
				if (in.drift || in.menu || in.horn || in.powerup || in.jump) {
					return false;
				}
				else if (in.tiltForward < 0.9) {
					// Do nothing
				}
				else {
					// up, up, down, down, left, right, left, right, B, A, start
					cout << "Code entered\n";
					konamiLock = true;
					konamiTime = timer.getCurrentTime();
					return true;
				}
			}
			counter--;
		}
		return false;
	}
	else {
		// Otherwise, stop checking
		return false;
	}
}

#endif
