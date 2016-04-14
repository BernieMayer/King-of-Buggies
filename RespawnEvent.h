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

	unsigned int getType() { return RESPAWN_EVENT; }

	RespawnEvent(int pNum, vec3 pos);
	~RespawnEvent();
};

#endif