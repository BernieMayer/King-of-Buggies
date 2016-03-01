#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <glm/glm.hpp>
#include "GameState.h"

#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <al/al.h>
#include <al/alc.h>
#include <cstdio>

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
	SoundManager(GameState state);
	bool loadMedia();
	void updateSounds(GameState state);
	void playAccelSound(GameState state, int playerNum);
	void playSound(float file , vec3 location, float volume);
	void adjustPitch(int, float);
	void adjustVolume(int, float);
	void stopSound(int);
	void initOpenAL();
	void initSDL(GameState state);
private:
	float map(float value, float min, float max, float newMin, float newMax);
	int lowestIdleVolume = 10;
};

#endif // SOUNDMANAGER_H