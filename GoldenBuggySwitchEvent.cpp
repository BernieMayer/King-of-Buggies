#ifndef GOLDENBUGGYSWITCHEVENT_CPP
#define GOLDENBUGGYSWITCHEVENT_CPP

#include "GoldenBuggySwitchEvent.h"


GoldenBuggySwitchEvent::GoldenBuggySwitchEvent(int oGB, int nGB, vec3 pos) 
{
	oldGB = oGB;
	newGB = nGB;
	gbPos = pos;
}

unsigned int GoldenBuggySwitchEvent::getType() {
	return GOLDEN_BUGGY_SWITCH_EVENT;
}


#endif
