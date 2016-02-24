#ifndef PLAYERINFO_CPP
#define PLAYERINFO_CPP

#include "PlayerInfo.h"

PlayerInfo::PlayerInfo() :	powerup(0), powerupCounter(0), numCoins(0), 
							goldenBuggie(false), timeGolden(0.f)
{

}

PlayerInfo::PlayerInfo(unsigned int _renderID, unsigned int _physicsID, unsigned int* _wheelRenderIDs):
powerup(0), powerupCounter(0), numCoins(0),
goldenBuggie(false), timeGolden(0.f)
{
	renderID = _renderID;
	physicsID = _physicsID;

	for (unsigned int i = 0; i < 4; i++)
	{
		wheelRenderIDs[i] = _wheelRenderIDs[i];
		wheelTransforms[i] = mat4(1.f);
	}

}

unsigned int PlayerInfo::getWheelRenderID(unsigned int wheelNum)
{
	if (wheelNum < 4)
		return wheelRenderIDs[wheelNum];
	else
		return 100000;
}

mat4 PlayerInfo::getWheelTransform(unsigned int wheelNum)
{
	if (wheelNum < 4)
		return wheelTransforms[wheelNum];
	else
		return mat4(1.f);
}

void PlayerInfo::setWheelTransform(unsigned int wheelNum, const mat4& transform)
{
	if (wheelNum < 4)
		wheelTransforms[wheelNum] = transform;
}

#endif // PLAYERINFO_CPP