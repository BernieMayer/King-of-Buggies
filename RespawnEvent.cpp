#ifndef RESPAWNEVENT_CPP
#define RESPAWNEVENT_CPP

#include "RespawnEvent.h"


RespawnEvent::RespawnEvent(int pNum, vec3 pos)
{
	playerNum = pNum;
	respawnLocation = pos;
}


RespawnEvent::~RespawnEvent()
{
}

#endif