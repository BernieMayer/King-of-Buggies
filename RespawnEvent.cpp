#ifndef RESPAWNEVENT_CPP
#define RESPAWNEVENT_CPP

#include "RespawnEvent.h"


RespawnEvent::RespawnEvent(int pNum, vec3 nPos, vec3 oPos)
{
	playerNum = pNum;
	respawnLocation = nPos;
	oldLocation = oPos;

}


RespawnEvent::~RespawnEvent()
{
}

#endif