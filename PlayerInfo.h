#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <glm/glm.hpp>
#include "Entity.h"
#include <vector>

class PlayerInfo: public Entity {
public:
	PlayerInfo();
	PlayerInfo(unsigned int renderID, unsigned int physicsID, unsigned int* wheelRenderIDs, vec3 colour, unsigned int texID);
	bool isGoldenBuggy();
	void setGoldenBuggy(bool val);
	bool isAI();
	void setAI(bool value);

	int getScore();
	void incrementScore();

	unsigned int getWheelRenderID(unsigned int wheelNum);
	mat4 getWheelTransform(unsigned int wheelNum);
	void setWheelTransform(unsigned int wheelNum, const mat4& transform);
	void setTransform(const mat4& transform, float time);
	
	void setColour(vec3 colour) { vehicleColour = colour; }
	vec3 getColour();
	unsigned int getTextureID();

	void setFSpeed(float speed) { fSpeed = speed; };
	float getFSpeed() { return fSpeed; };
	void setSSpeed(float speed) { sSpeed = speed; };
	float getSSpeed() { return sSpeed; };
	vec3 getVelocity();
	bool getForwardsGear() { return forwardsGear; }
	void setForwardsGear(bool b) { forwardsGear = b; }
	float getEngineSpeed() { return engineSpeed; }
	void setEngineSpeed(float speed) { engineSpeed = speed; }
	bool getInAir() { return inAir; }
	void setInAir(bool air) { inAir = air; }

	void addPowerUp(int id);
	int usePowerUp();
	void removePowerup();
	int getCurrentPowerup();

	
	void removeEnergyForNitro(float nitro);
	void setEnergyForNitro(float nitro);
	float getEnergyForNitro();

protected:
	unsigned int wheelRenderIDs[4];
	mat4 wheelTransforms [4];
	vec3 velocity;

	float energyForNitro;
	int powerup;
	int numCoins;
	int score;
	bool goldenBuggy;
	bool ai;
	float timeGolden;
	float fSpeed;
	float sSpeed;
	bool forwardsGear;
	float engineSpeed;
	bool inAir;

	vec3 vehicleColour;
	unsigned int textureID;
};

#endif // PLAYERINFO_H