#ifndef GOLDENBUGGYSWITCHEVENT_H
#define GOLDENBUGGYSWITCHEVENT_H

#include <glm/glm.hpp>
#include "Events.h"

using namespace glm;

class GoldenBuggySwitchEvent :
	public Event
{
public:
	GoldenBuggySwitchEvent(int oGB, int nGB, vec3 pos);

	int oldGB, newGB;
	vec3 gbPos;
	unsigned int getType();
};

#endif

