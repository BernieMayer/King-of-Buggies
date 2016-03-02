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

#include <stdio.h>
#include <iostream>

using namespace glm;

class SoundManager {
public:
	SoundManager();
	SoundManager(GameState state);
	void updateSounds(GameState state, Input inputs[]);
	void playBumpSound(vec3 pos);
	void playDingSound(vec3 pos);
	
private:
	float map(float value, float min, float max, float newMin, float newMax);
	int lowestIdleVolume = 10;
	void initOpenAL(GameState state);
	void initListener(GameState state);
	void startMusic(GameState state);
	void startEngineSounds(GameState state);
	void updateListener(GameState state);
	void updateMusic(GameState state);
	void updateEngineSounds(GameState state, Input inputs[]);
	void loadWavToBuf(string fileName, ALuint* source, ALuint *buffer);
	void playSecret(GameState state);
	void playSecret2(GameState state);
	void updateMusicPitch(GameState state, Input input);
	ALfloat* vec3ToALfloat(vec3 vec);
	ALfloat* vec3ToALfloat(vec3 vector1, vec3 vector2);
	ALuint musicSource;
	ALuint engineSources[5];
	ALuint listener;

	float musicVolume = 1.0;
	float idleEngineVolume = 0.1;
	float idleEnginePitch = 1.0;

	bool secretPlaying = false;
	bool secret2Unlocked = false;
	bool driftDown = false;
};

#endif // SOUNDMANAGER_H