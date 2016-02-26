#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <glm/glm.hpp>
#include "GameState.h"

using namespace glm;

class SoundManager {
public:
	//TODO: add speed trait to PlayerInfo
	void playAccelSound(GameState state, int playerNum);
	void playSound(float file , vec3 location, float volume);
	void adjustPitch(int, float);
	void adjustVolume(int, float);
	void stopSound(int);
};

#endif // SOUNDMANAGER_H