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
	}

}

unsigned int PlayerInfo::getWheelRenderID(unsigned int wheelNum)
{
	if (wheelNum < 4)
		return wheelRenderIDs[wheelNum];
	else
		return 100000;
}


#endif // PLAYERINFO_CPP