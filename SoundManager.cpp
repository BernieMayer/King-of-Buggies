#ifndef SOUNDMANAGER_CPP
#define SOUNDMANAGER_CPP
#include "SoundManager.h"

Mix_Music *gMusic;
Mix_Chunk *idleSound;
bool idlePlaying;

SoundManager::SoundManager() {
}



SoundManager::SoundManager(GameState state) {
	initOpenAL(state);
	//initSDL(state);
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

void SoundManager::initListener(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *ListenerPos = vec3ToALfloat(p1->getPos());
	ALfloat *ListenerVel = vec3ToALfloat(p1->getVelocity());
	ALfloat *ListenerOri = vec3ToALfloat(p1->getForward(), p1->getUp());

	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

void SoundManager::startMusic(GameState state) {
	ALuint buffer;

	loadWavToBuf("Dogsong.wav", &musicSource, &buffer);

	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcei(musicSource, AL_BUFFER, buffer);
	alSourcef(musicSource, AL_PITCH, 1.0f);
	alSourcef(musicSource, AL_GAIN, 1.0f);
	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
	alSourcei(musicSource, AL_LOOPING, AL_TRUE);

	alSourcePlay(musicSource);
}

void SoundManager::startEngineSounds(GameState state) {
	for (int i = 0; i < state.numberOfPlayers(); i++) {
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

void SoundManager::updateListener(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *ListenerPos = vec3ToALfloat(p1->getPos());
	ALfloat *ListenerVel = vec3ToALfloat(p1->getVelocity());
	ALfloat *ListenerOri = vec3ToALfloat(p1->getForward(), p1->getUp());

	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

void SoundManager::updateMusic(GameState state) {
	PlayerInfo* p1 = state.getPlayer(0);

	ALfloat *SourcePos = vec3ToALfloat(p1->getPos());
	ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity());

	alSourcefv(musicSource, AL_POSITION, SourcePos);
	alSourcefv(musicSource, AL_VELOCITY, SourceVel);
}

void SoundManager::updateEngineSounds(GameState state, Input inputs[]) {
	for (int i = 0; i < state.numberOfPlayers(); i++) {
		PlayerInfo* player = state.getPlayer(i);
	
		bool forwardsGear = player->getForwardsGear();

		float accelInput = 0;
		if (forwardsGear) {
			accelInput = map(inputs[i].forward, 0, 1, 1, 20);
		}
		else {
			accelInput = map(inputs[i].backward, 0, 1, 1, 20);
		}

		ALfloat *SourcePos = vec3ToALfloat(player->getPos());
		ALfloat *SourceVel = vec3ToALfloat(player->getVelocity());

		float pitchMod = accelInput / 14;
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

ALfloat* SoundManager::vec3ToALfloat(vec3 vec) {
	ALfloat f[] = { vec.x, vec.y, vec.z };
	return f;
}

ALfloat* SoundManager::vec3ToALfloat(vec3 vector1, vec3 vector2) {
	ALfloat f[] = { vector1.x, vector1.y, vector1.z, vector2.x, vector2.y, vector2.z };
	return f;
}

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

void SoundManager::initSDL(GameState state) {
	gMusic = NULL;
	idleSound;
	bool success = true;
	idlePlaying = false;

	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}

	// Init SDL_Mixer
	// Sound frequency, sample format, number of hardware channels (2 = stereo),
	// size of chucks of audio in bytes (may need to play with this number)
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		cout << "SDL_mixer could not intialize! SDL_mixer Error: " << Mix_GetError() << "\n";
		success = false;
	}

	if (success) {
		success = loadMedia();
	}

	if (success) {
		// Number is number of times music is repeated
		// -1 means loop
		Mix_PlayMusic(gMusic, -1);

		// Sound for player 1
		Mix_PlayChannel(2, idleSound, -1);
		Mix_Volume(2, lowestIdleVolume);

		int numPlayers = state.numberOfPlayers();

		for (int i = 1; i < numPlayers; i++) {
			Mix_PlayChannel(i + 2, idleSound, -1);
			Mix_Volume(i + 2, lowestIdleVolume);

			vec3 player1Pos = state.getPlayer(0)->getPos();
			vec3 otherPlayerPos = state.getPlayer(i)->getPos();

			float distance = length(player1Pos - otherPlayerPos);
			if (distance > 255) {
				distance = 255;
			}
			Mix_SetDistance(i + 2, distance);
		}
	}
}

bool SoundManager::loadMedia() {
	bool success = true;

	// Load music
	gMusic = Mix_LoadMUS("Sounds/Dogsong.wav");
	if (gMusic == NULL) {
		cout << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << "\n";
		success = false;
	}

	// Load idle
	idleSound = Mix_LoadWAV("Sounds/Idle.wav");
	if (idleSound == NULL) {
		cout << "Failed to load idle sound! SDL_mixer Error: " << Mix_GetError() << "\n";
		success = false;
	}

	return success;
}

void SoundManager::updateSounds(GameState state, Input inputs[]) {
	updateListener(state);
	updateMusic(state);
	updateEngineSounds(state, inputs);
	
	int numPlayers = state.numberOfPlayers();

	// Get player 1
	PlayerInfo* player1 = state.getPlayer(0);
	float player1Speed = abs(player1->getFSpeed());

	// TODO: change to be based on input so engine will make reving sound even if
	// driveing into a wall with 0 speed
	int volume = (int)map(player1Speed, 0, 16, lowestIdleVolume, 128);
	Mix_Volume(2, volume);

	for (int i = 1; i < numPlayers; i++) {
		PlayerInfo* player = state.getPlayer(i);
		float playerSpeed = abs(player->getFSpeed());
		volume = (int)map(playerSpeed, 0, 16, lowestIdleVolume, 128);
		Mix_Volume(i + 2, volume);

		vec3 player1Pos = state.getPlayer(0)->getPos();
		vec3 otherPlayerPos = state.getPlayer(i)->getPos();

		float distance = length(player1Pos - otherPlayerPos);
		if (distance > 255) {
			distance = 255;
		}
		Mix_SetDistance(i + 2, distance);
	}
}

/**
* Takes a value between two numbers and converts it to another value between two different numbers
* maintaining its proportional place between the first two numbers.
*/
float SoundManager::map(float value, float min, float max, float newMin, float newMax) {
	return (value - min) * ((newMax - newMin) / (max - min)) + newMin;
}

#endif
