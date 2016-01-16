#pragma once
class InputManager
{
public:
	InputManager();
	~InputManager();
private:
	// Used to notify game manager of the amount of forward/backward acceleration 
	// the player is requesting
	void notifyForward(float accel);
	// Used to notify game manager of the amount of turning the player has requested
	void notifyTurn(float accel);
	// Used to notify game manager that the player has requested to start a drift
	void notifyDriftStart();
	// Used to notify game manager that the player has requested to end a drift
	void notifyDriftEnd();
	// Used to notify game manger that the player has requested to use a power-up
	void notifyPowerup();

	// Used to notify game manager that the player has requested to tilt camera up/down
	void notifyTiltCameraV(float accel);
	// Used to notify game manager that the player has requested to tilt camera left/right
	void notifyTiltCameraH(float accel);

	// Used to notify game manager that the plaayer has pressed the menu button
	void menuButtonPressed();
};

