#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <glm/glm.hpp>
#include "GameState.h"
#include "Input.h"

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
	void updateSounds(GameState state, Input inputs[]);
	void playAccelSound(GameState state, int playerNum);
	void playSound(float file , vec3 location, float volume);
	void adjustPitch(int, float);
	void adjustVolume(int, float);
	void stopSound(int);
	void initOpenAL(GameState state);
	void initSDL(GameState state);
private:
	float map(float value, float min, float max, float newMin, float newMax);
	int lowestIdleVolume = 10;
	void initListener(GameState state);
	void startMusic(GameState state);
	void startEngineSounds(GameState state);
	void updateListener(GameState state);
	void updateMusic(GameState state);
	void updateEngineSounds(GameState state, Input inputs[]);
	void loadWavToBuf(string fileName, ALuint* source, ALuint* buffer);
	ALfloat* vec3ToALfloat(vec3 vec);
	ALfloat* vec3ToALfloat(vec3 vector1, vec3 vector2);
	ALuint musicSource;
	ALuint engineSources[5];
	ALuint listener;

	float idleEngineVolume = 0.1;
	float idleEnginePitch = 1.0;
};

#endif // SOUNDMANAGER_H