#ifndef RESPAWNEVENT_H
#define RESPAWNEVENT_H

#include "Events.h"
#include <glm/glm.hpp>

using namespace glm;

class RespawnEvent :
	public Event
{
public:
	int playerNum;
	vec3 respawnLocation;
	vec3 oldLocation;

	unsigned int getType() { return RESPAWN_EVENT; }

	RespawnEvent(int pNum, vec3 nPos, vec3 oPos);
	~RespawnEvent();
};

#endif