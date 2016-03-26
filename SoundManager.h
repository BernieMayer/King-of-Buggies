#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <glm/glm.hpp>
#include "GameState.h"
#include "Input.h"
#include "Definitions.h"

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
	void startSounds(GameState state);
	void updateSounds(GameState state, Input inputs[]);
	void playBumpSound(vec3 pos, float volume);
	void playDingSound(vec3 pos);
	void playWinSound(vec3 pos);
	void playLossSound(vec3 pos);

	void startMenuSong(GameState state);
	void updateMenuSong(GameState state);
	void stopMenuSong();

	void initListener(GameState state);
	void startMusic(GameState state);
	void startEngineSounds(GameState state);
	
private:
	float map(float value, float min, float max, float newMin, float newMax);
	int lowestIdleVolume = 10;
	int menuSongCounter = 0;
	int menuSongCounterMax = 15;
	void initOpenAL(GameState state);
	
	float distanceVolumeAdjuster(float volume, vec3 pos);
	void updateListener(GameState state);
	void updateMusic(GameState state);
	void updateEngineSounds(GameState state, Input inputs[]);
	void updateHonks(GameState state, Input inputs[]);
	void startFuse(GameState state);
	void updateFuses(GameState state);
	void stopFuse(int fuseNum);
	void loadWavToBuf(string fileName, ALuint* source, ALuint *buffer);
	void playSound(string fileName, vec3 pos, float volume);
	void playSecretMusic(GameState state);
	void playPauseSong(GameState state);
	void updateMusicPitch(GameState state, Input input);
	void cleanOneTimeUseSources();
	vector<ALfloat> vec3ToALfloat(vec3 vec);
	vector<ALfloat> vec3ToALfloat(vec3 vector1, vec3 vector2);
	ALuint musicBuffer;
	ALuint musicSource;
	ALuint engineSources[MAX_PLAYERS];
	ALuint listener;
	vector<ALuint> oneTimeUseBuffers;
	vector<ALuint> oneTimeUseSources;
	vec3 listenerPos;
	// Higher and distance will have less of an effect on volume, lower will increase the effect
	float distanceDivider = 15.0f;

	float musicVolume = 1.0;
	float idleEngineVolume = 0.08;
	float idleEnginePitch = 1.0;

	vector<bool> honking;
	vector<ALuint> honkSources;

	vector<ALuint> fuseBuffers;
	vector<ALuint> fuseSources;

	bool paused = false;
	bool secretPlaying = false;
	int lastSecretPlayed = -1;
};

#endif // SOUNDMANAGER_H