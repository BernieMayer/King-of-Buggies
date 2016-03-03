#ifndef SOUNDMANAGER_CPP
#define SOUNDMANAGER_CPP
#include "SoundManager.h"

SoundManager::SoundManager() {
}



SoundManager::SoundManager(GameState state) {
	initOpenAL(state);
}

void SoundManager::initOpenAL(GameState state) {
	ALCdevice* device = alcOpenDevice(NULL);
	if (!device) {
		cout << "ALCDevice creation failed\n";
	}
	ALCcontext* context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	initListener(state);
	startMusic(state);
	startEngineSounds(state);
}

/*
 * Initializes the listener at the location of the player
 */
void SoundManager::initListener(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *ListenerPos = vec3ToALfloat(p1->getPos());
	ALfloat *ListenerVel = vec3ToALfloat(p1->getVelocity());
	ALfloat *ListenerOri = vec3ToALfloat(p1->getForward(), p1->getUp());

	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

/*
 * Initializes the music to be playing at the location of the player
 */
void SoundManager::startMusic(GameState state) {
	ALuint buffer;

	loadWavToBuf("Dogsong.wav", &musicSource, &buffer);

	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcei(musicSource, AL_BUFFER, buffer);
	alSourcef(musicSource, AL_PITCH, 1.0f);
	alSourcef(musicSource, AL_GAIN, musicVolume);
	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
	alSourcei(musicSource, AL_LOOPING, AL_TRUE);

	alSourcePlay(musicSource);
}

/*
 * Initializes engines sounds to play at the location of players
 */
void SoundManager::startEngineSounds(GameState state) {
	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		ALuint buffer;

		loadWavToBuf("Idle.wav", &engineSources[i], &buffer);

		PlayerInfo* player = state.getPlayer(i);

		ALfloat *SourcePos = vec3ToALfloat(player->getPos());
		ALfloat *SourceVel = vec3ToALfloat(player->getVelocity());

		alSourcei(engineSources[i], AL_BUFFER, buffer);
		alSourcef(engineSources[i], AL_PITCH, 1.0f);
		alSourcef(engineSources[i], AL_GAIN, idleEngineVolume);
		alSourcefv(engineSources[i], AL_POSITION, SourcePos);
		alSourcefv(engineSources[i], AL_VELOCITY, SourceVel);
		alSourcei(engineSources[i], AL_LOOPING, AL_TRUE);

		alSourcePlay(engineSources[i]);
	}
}


/*
 * Updates the location of the listener so it is still the location of the player
 */
void SoundManager::updateListener(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *ListenerPos = vec3ToALfloat(p1->getPos());
	ALfloat *ListenerVel = vec3ToALfloat(p1->getVelocity());
	ALfloat *ListenerOri = vec3ToALfloat(p1->getForward(), p1->getUp());

	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

/*
 * Updates the music so it is still playing at the location of the player
 */
void SoundManager::updateMusic(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
}

/*
 * Updates the engines sounds' location as well as volume and pitch based
 * on input
 */
void SoundManager::updateEngineSounds(GameState state, Input inputs[]) {
	for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
		PlayerInfo* player = state.getPlayer(i);
	
		bool forwardsGear = player->getForwardsGear();

		float accelInput = 0;
		accelInput = abs(player->getWheelRotationSpeed());

		ALfloat *SourcePos = vec3ToALfloat(player->getPos());
		ALfloat *SourceVel = vec3ToALfloat(player->getVelocity());

		float pitchMod = map(accelInput, 0, 16, 1, 2);
		if (pitchMod < 1) {
			pitchMod = 1;
		}
		alSourcef(engineSources[i], AL_PITCH, idleEnginePitch * pitchMod);
		
		alSourcef(engineSources[i], AL_GAIN, idleEngineVolume * accelInput);
		alSourcefv(engineSources[i], AL_POSITION, SourcePos);
		alSourcefv(engineSources[i], AL_VELOCITY, SourceVel);
		alSourcei(engineSources[i], AL_LOOPING, AL_TRUE);
	}
}

/*
 * Converts a vec3 to an array of ALfloats
 */
ALfloat* SoundManager::vec3ToALfloat(vec3 vec) {
	ALfloat f[] = { vec.x, vec.y, vec.z };
	return f;
}

/*
 * Converts two vec3s to an aray of ALfloats
 */
ALfloat* SoundManager::vec3ToALfloat(vec3 vector1, vec3 vector2) {
	ALfloat f[] = { vector1.x, vector1.y, vector1.z, vector2.x, vector2.y, vector2.z };
	return f;
}

/*
 * Loads a wav file to the given buffer and loads that buffer into the source
 * No need to add "Sounds\" to the file name. This method will do that
 * Just give "sound.wav"
 */
void SoundManager::loadWavToBuf(string fileName, ALuint* source, ALuint* buffer) {
	string name = "Sounds/" + fileName;
	const char* fileNameChar = name.c_str();
	FILE* fp = fopen(fileNameChar, "rb");
	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
	{
		cout << "No RIFF\n";
	}

	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
	{
		cout << "Not WAVE\n";
	}

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
	{
		cout << "Not fmt\n";
	}

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a')
	{
		cout << "Missing DATA\n";
	}

	fread(&dataSize, sizeof(DWORD), 1, fp);

	unsigned char *buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);

	ALuint frequency = sampleRate;
	ALenum format = 0;

	alGenBuffers(1, buffer);
	alGenSources(1, source);

	if (bitsPerSample == 8) {
		if (channels == 1) {
			format = AL_FORMAT_MONO8;
		}
		else if (channels == 2) {
			format = AL_FORMAT_STEREO8;
		}
	}
	else if (bitsPerSample == 16) {
		if (channels == 1) {
			format = AL_FORMAT_MONO16;
		}
		else if (channels == 2) {
			format = AL_FORMAT_STEREO16;
		}
	}

	alBufferData(*buffer, format, buf, dataSize, frequency);
}

/*
 * Loads and plays a bump sound once at the given position
 */
void SoundManager::playBumpSound(vec3 pos) {
	ALuint bumpSource;
	ALuint buffer;

	loadWavToBuf("Bump.wav", &bumpSource, &buffer);

	ALfloat *SourcePos = vec3ToALfloat(pos);

	alSourcei(bumpSource, AL_BUFFER, buffer);
	alSourcef(bumpSource, AL_PITCH, 1.0f);
	alSourcef(bumpSource, AL_GAIN, 1.0f);
	alSourcefv(bumpSource, AL_POSITION, SourcePos);
	alSourcei(bumpSource, AL_LOOPING, AL_FALSE);

	alSourcePlay(bumpSource);
}

/*
* Loads and plays a bump sound once at the given position
*/
void SoundManager::playDingSound(vec3 pos) {
	ALuint dingSource;
	ALuint buffer;

	loadWavToBuf("Ding.wav", &dingSource, &buffer);

	ALfloat *SourcePos = vec3ToALfloat(pos);

	alSourcei(dingSource, AL_BUFFER, buffer);
	alSourcef(dingSource, AL_PITCH, 1.0f);
	alSourcef(dingSource, AL_GAIN, 1.0f);
	alSourcefv(dingSource, AL_POSITION, SourcePos);
	alSourcei(dingSource, AL_LOOPING, AL_FALSE);

	alSourcePlay(dingSource);
}

void SoundManager::playSecret(GameState state) {
	alSourcePause(musicSource);
	ALuint buffer;

	loadWavToBuf("Secret.wav", &musicSource, &buffer);

	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcei(musicSource, AL_BUFFER, buffer);
	alSourcef(musicSource, AL_PITCH, 1.0f);
	alSourcef(musicSource, AL_GAIN, musicVolume);
	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
	alSourcei(musicSource, AL_LOOPING, AL_FALSE);

	alSourcePlay(musicSource);
}

void SoundManager::playSecret2(GameState state) {
	alSourcePause(musicSource);
	ALuint buffer;

	loadWavToBuf("Dogstorm.wav", &musicSource, &buffer);

	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcei(musicSource, AL_BUFFER, buffer);
	alSourcef(musicSource, AL_PITCH, 1.0f);
	alSourcef(musicSource, AL_GAIN, musicVolume);
	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
	alSourcei(musicSource, AL_LOOPING, AL_FALSE);

	alSourcePlay(musicSource);
}

void SoundManager::updateMusicPitch(GameState state, Input input) {
	PlayerInfo* player = state.getPlayer(0);

	bool forwardsGear = player->getForwardsGear();

	float accelInput = 0;
	if (forwardsGear) {
		accelInput = map(input.forward, 0, 1, 0.5, 2);
	}
	else {
		accelInput = map(input.backward, 0, 1, 0.5, 2);
	}

	alSourcef(musicSource, AL_PITCH, 1 * accelInput);
}

/*
 * Updates all sounds
 */
void SoundManager::updateSounds(GameState state, Input inputs[]) {
	updateListener(state);
	updateMusic(state);
	updateEngineSounds(state, inputs);
	if (inputs[0].powerup) {
		playBumpSound(state.getPlayer(0)->getPos());
	}
	else if (inputs[0].menu) {
		playDingSound(state.getPlayer(0)->getPos());
	}
	else if (inputs[0].drift && !secretPlaying && !secret2Unlocked && !driftDown) {
		secretPlaying = true;
		driftDown = true;
		playSecret(state);
	}
	else if (inputs[0].drift && secretPlaying && !secret2Unlocked && !driftDown) {
		secretPlaying = false;
		driftDown = true;
		alSourcePause(musicSource);
		startMusic(state);
	}
	else if (inputs[0].drift && !secretPlaying && secret2Unlocked && !driftDown) {
		secretPlaying = true;
		driftDown = true;
		playSecret2(state);
	}
	else if (inputs[0].drift && secretPlaying && secret2Unlocked && !driftDown) {
		secretPlaying = false;
		secret2Unlocked = false;
		driftDown = true;
		alSourcePause(musicSource);
		startMusic(state);
	}
	else if (!inputs[0].drift && driftDown) {
		driftDown = false;
	}

	ALint musicState;
	alGetSourcei(musicSource, AL_SOURCE_STATE, &musicState);
	if (musicState == AL_STOPPED && secretPlaying && !secret2Unlocked) {
		secretPlaying = false;
		secret2Unlocked = true;
		alSourcePause(musicSource);
		startMusic(state);
	}
	else if (musicState == AL_STOPPED && secretPlaying && secret2Unlocked) {
		secretPlaying = false;
		secret2Unlocked = false;
		alSourcePause(musicSource);
		startMusic(state);
	}

	// Feel free to remove this. It has no purpose, just sounds funny
	//updateMusicPitch(state, inputs[0]);
}

/**
* Takes a value between two numbers and converts it to another value between two different numbers
* maintaining its proportional place between the first two numbers.
*/
float SoundManager::map(float value, float min, float max, float newMin, float newMax) {
	return (value - min) * ((newMax - newMin) / (max - min)) + newMin;
}

#endif
