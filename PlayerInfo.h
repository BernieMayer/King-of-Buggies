#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <glm/glm.hpp>

class PlayerInfo {
public:
	bool isGoldenBuggie();

private:
	vec3 forward;
	vec3 up;
};

#endif // PLAYERINFO_H