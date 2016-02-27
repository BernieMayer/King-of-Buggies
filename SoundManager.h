#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <glm/glm.hpp>
#include "GameState.h"

#include <SDL.h>
#include <SDL_mixer.h>
// Not sure what this does, or why we need it
// but we do
#undef main

#include <stdio.h>
#include <iostream>

using namespace glm;

class SoundManager {
public:
	SoundManager();
	bool loadMedia();
	void playAccelSound(GameState state, int playerNum);
	void playSound(float file , vec3 location, float volume);
	void adjustPitch(int, float);
	void adjustVolume(int, float);
	void stopSound(int);
};

#endif // SOUNDMANAGER_H