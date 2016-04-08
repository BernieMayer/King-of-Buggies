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
		initSuccess = false;
	}

	if (initSuccess) {
		ALCcontext* context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);

		alDistanceModel(AL_LINEAR_DISTANCE);
		alDopplerFactor(1.0f);

		initListener(state);
	}
}

void SoundManager::startSounds(GameState state) {
	if (initSuccess) {
		startMusic(state);
		startEngineSounds(state);
	}
}

/*
 * Initializes the listener at the location of the player
 */
void SoundManager::initListener(GameState state) {
	if (initSuccess) {
		ALfloat *ListenerPos;
		ALfloat *ListenerVel;
		if (state.numberOfPlayers() > 0) {
			PlayerInfo* p1 = state.getPlayer(0);

			ListenerPos = vec3ToALfloat(p1->getPos()).data();
			ListenerVel = vec3ToALfloat(p1->getVelocity()).data();
		}
		else {
			ListenerPos = vec3ToALfloat(vec3(0.f, 5.f, 0.f)).data();
			ListenerVel = vec3ToALfloat(vec3(0.0f, 0.0f, 0.0f)).data();
		}

		alListenerfv(AL_POSITION, ListenerPos);
		alListenerfv(AL_VELOCITY, ListenerVel);

		for (int i = 0; i < MAX_PLAYERS; i++) {
			honking.push_back(false);
			honkSources.push_back(0);
			honkBuffers.push_back(0);
		}
	}
}

void SoundManager::startMenuSong(GameState state) {
	if (initSuccess) {
		loadWavToBuf("Menu.wav", &musicSource, &musicBuffer);

		ALfloat *SourcePos;
		ALfloat *SourceVel;

		if (state.numberOfPlayers() > 0) {
			PlayerInfo* p1 = state.getPlayer(0);

			SourcePos = vec3ToALfloat(p1->getPos()).data();
			SourceVel = vec3ToALfloat(p1->getVelocity()).data();
		}
		else {
			SourcePos = vec3ToALfloat(vec3(0.f, 5.f, 0.f)).data();
			SourceVel = vec3ToALfloat(vec3(0.0f, 0.0f, 0.0f)).data();
		}

		alSourcei(musicSource, AL_BUFFER, musicBuffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(musicSource);
	}
}

void SoundManager::updateMenuSong(GameState state) {
	if (initSuccess) {
		ALfloat *SourcePos;
		ALfloat *SourceVel;

		if (state.numberOfPlayers() > 0) {
			PlayerInfo* p1 = state.getPlayer(0);

			SourcePos = vec3ToALfloat(p1->getPos()).data();
			SourceVel = vec3ToALfloat(p1->getVelocity()).data();
		}
		else {
			SourcePos = vec3ToALfloat(vec3(0.f, 5.f, 0.f)).data();
			SourceVel = vec3ToALfloat(vec3(0.0f, 0.0f, 0.0f)).data();
		}

		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);

		ALint musicState;
		alGetSourcei(musicSource, AL_SOURCE_STATE, &musicState);
		if (musicState == AL_STOPPED) {
			menuSongCounter++;
			if (menuSongCounter != menuSongCounterMax) {
				if (menuSongCounter == menuSongCounterMax + 1) {
					alDeleteBuffers(1, &musicBuffer);
					alDeleteSources(1, &musicSource);

					loadWavToBuf("Menu.wav", &musicSource, &musicBuffer);

					ALfloat *SourcePos;
					ALfloat *SourceVel;

					if (state.numberOfPlayers() > 0) {
						PlayerInfo* p1 = state.getPlayer(0);

						SourcePos = vec3ToALfloat(p1->getPos()).data();
						SourceVel = vec3ToALfloat(p1->getVelocity()).data();
					}
					else {
						SourcePos = vec3ToALfloat(vec3(0.f, 5.f, 0.f)).data();
						SourceVel = vec3ToALfloat(vec3(0.0f, 0.0f, 0.0f)).data();
					}

					alSourcei(musicSource, AL_BUFFER, musicBuffer);
					alSourcef(musicSource, AL_PITCH, 1.0f);
					alSourcef(musicSource, AL_GAIN, musicVolume);
					alSourcefv(musicSource, AL_POSITION, SourcePos);
					alSourcefv(musicSource, AL_VELOCITY, SourceVel);
					alSourcei(musicSource, AL_LOOPING, AL_FALSE);

					alSourcePlay(musicSource);
				}
				else
				{
					alSourcePlay(musicSource);
				}
			}
			else {
				alDeleteBuffers(1, &musicBuffer);
				alDeleteSources(1, &musicSource);

				loadWavToBuf("Menu2.wav", &musicSource, &musicBuffer);

				ALfloat *SourcePos;
				ALfloat *SourceVel;

				if (state.numberOfPlayers() > 0) {
					PlayerInfo* p1 = state.getPlayer(0);

					SourcePos = vec3ToALfloat(p1->getPos()).data();
					SourceVel = vec3ToALfloat(p1->getVelocity()).data();
				}
				else {
					SourcePos = vec3ToALfloat(vec3(0.f, 5.f, 0.f)).data();
					SourceVel = vec3ToALfloat(vec3(0.0f, 0.0f, 0.0f)).data();
				}

				alSourcei(musicSource, AL_BUFFER, musicBuffer);
				alSourcef(musicSource, AL_PITCH, 1.0f);
				alSourcef(musicSource, AL_GAIN, musicVolume);
				alSourcefv(musicSource, AL_POSITION, SourcePos);
				alSourcefv(musicSource, AL_VELOCITY, SourceVel);
				alSourcei(musicSource, AL_LOOPING, AL_FALSE);

				alSourcePlay(musicSource);
			}
		}
	}
}

void SoundManager::stopMenuSong() {
	if (initSuccess) {
		alDeleteBuffers(1, &musicBuffer);
		alDeleteSources(1, &musicSource);
	}
}

/*
 * Initializes the music to be playing at the location of the player
 */
void SoundManager::startMusic(GameState state) {
	if (initSuccess) {
		loadWavToBuf("Dogsong.wav", &musicSource, &musicBuffer);

		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcei(musicSource, AL_BUFFER, musicBuffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_TRUE);

		alSourcePlay(musicSource);
	}
}

float SoundManager::distanceVolumeAdjuster(float volume, vec3 pos) {
	float distanceVolumeMod = length(listenerPos - pos) / distanceDivider;
	// Ensures that volume the denominator will never be less than 1
	// thus the volume returned will never be greater than the input volume
	distanceVolumeMod += 1.0f;
	distanceVolumeMod = 1 / distanceVolumeMod;

	return distanceVolumeMod * volume;
}

/*
 * Initializes engines sounds to play at the location of players
 */
void SoundManager::startEngineSounds(GameState state) {
	if (initSuccess) {
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
			loadWavToBuf("Idle.wav", &engineSources[i], &engineBuffers[i]);

			PlayerInfo* player = state.getPlayer(i);

			ALfloat *SourcePos = vec3ToALfloat(player->getPos()).data();
			ALfloat *SourceVel = vec3ToALfloat(player->getVelocity()).data();

			float volume = distanceVolumeAdjuster(idleEngineVolume, state.getPlayer(i)->getPos());

			alSourcei(engineSources[i], AL_BUFFER, engineBuffers[i]);
			alSourcef(engineSources[i], AL_PITCH, 1.0f);
			alSourcef(engineSources[i], AL_GAIN, volume);
			alSourcefv(engineSources[i], AL_POSITION, SourcePos);
			alSourcefv(engineSources[i], AL_VELOCITY, SourceVel);
			alSourcei(engineSources[i], AL_LOOPING, AL_TRUE);

			//alSourcei(engineSources[i], AL_REFERENCE_DISTANCE, )

			alSourcePlay(engineSources[i]);
		}
	}
}


/*
 * Updates the location of the listener so it is still the location of the player
 */
void SoundManager::updateListener(GameState state) {
	if (initSuccess) {
		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *ListenerPos = vec3ToALfloat(p1->getPos()).data();
		listenerPos = p1->getPos();
		ALfloat *ListenerVel = vec3ToALfloat(p1->getVelocity()).data();

		alListenerfv(AL_POSITION, ListenerPos);
		alListenerfv(AL_VELOCITY, ListenerVel);
	}
}

/*
 * Updates the music so it is still playing at the location of the player
 */
void SoundManager::updateMusic(GameState state) {
	if (initSuccess) {
		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
	}
}

/*
 * Updates the engines sounds' location as well as volume and pitch based
 * on input
 */
void SoundManager::updateEngineSounds(GameState state, Input inputs[]) {
	if (initSuccess) {
		for (unsigned int i = 0; i < state.numberOfPlayers(); i++) {
			PlayerInfo* player = state.getPlayer(i);

			bool forwardsGear = player->getForwardsGear();

			float accelInput = 0;
			accelInput = abs(player->getEngineSpeed());

			ALfloat *SourcePos = vec3ToALfloat(player->getPos()).data();
			ALfloat *SourceVel = vec3ToALfloat(player->getVelocity()).data();

			float pitchMod = map(accelInput, 0, 600, 1, 2.0);
			if (pitchMod < 1) {
				pitchMod = 1;
			}
			alSourcef(engineSources[i], AL_PITCH, idleEnginePitch * pitchMod);


			accelInput = map(accelInput, 0, 600, 1, 16);

			float volume = distanceVolumeAdjuster(idleEngineVolume * accelInput, state.getPlayer(i)->getPos());

			alSourcef(engineSources[i], AL_GAIN, volume);
			alSourcefv(engineSources[i], AL_POSITION, SourcePos);
			alSourcefv(engineSources[i], AL_VELOCITY, SourceVel);
			alSourcei(engineSources[i], AL_LOOPING, AL_TRUE);

			if (inputs[i].horn) {
				if (!honking[i]) {
					loadWavToBuf("Honk.wav", &honkSources[i], &honkBuffers[i]);

					PlayerInfo* player = state.getPlayer(i);

					ALfloat *SourcePos = vec3ToALfloat(player->getPos()).data();
					ALfloat *SourceVel = vec3ToALfloat(player->getVelocity()).data();


					float volume = distanceVolumeAdjuster(1.0, state.getPlayer(i)->getPos());
					alSourcei(honkSources[i], AL_BUFFER, honkBuffers[i]);
					alSourcef(honkSources[i], AL_PITCH, 1.0);
					alSourcef(honkSources[i], AL_GAIN, volume);
					alSourcefv(honkSources[i], AL_POSITION, SourcePos);
					alSourcefv(honkSources[i], AL_VELOCITY, SourceVel);
					alSourcei(honkSources[i], AL_LOOPING, AL_TRUE);

					alSourcePlay(honkSources[i]);
					honking[i] = true;
				}
				else {
					PlayerInfo* p1 = state.getPlayer(i);

					ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
					ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

					alSourcefv(honkSources[i], AL_POSITION, SourcePos);
					alSourcefv(honkSources[i], AL_VELOCITY, SourceVel);
				}
			}
			else {
				if (honking[i]) {
					alSourceStop(honkSources[i]);
					alDeleteSources(1, &honkSources[i]);
					alDeleteBuffers(1, &honkBuffers[i]);

					honking[i] = false;
				}
			}
		}
	}
}

void SoundManager::startFuse(GameState state) {
	if (initSuccess) {
		fuseBuffers.push_back(0);
		fuseSources.push_back(0);

		loadWavToBuf("Fuse.wav", &fuseSources.back(), &fuseBuffers.back());

		// Get most recent powerup
		Powerup* bomb = state.getPowerup(state.numberOfPowerups() - 1);

		ALfloat *SourcePos = vec3ToALfloat(bomb->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(bomb->getVelocity()).data();

		float volume = distanceVolumeAdjuster(1.0f, bomb->getPos());

		alSourcei(fuseSources.back(), AL_BUFFER, fuseBuffers.back());
		alSourcef(fuseSources.back(), AL_PITCH, 1.0f);
		alSourcef(fuseSources.back(), AL_GAIN, volume);
		alSourcefv(fuseSources.back(), AL_POSITION, SourcePos);
		alSourcefv(fuseSources.back(), AL_VELOCITY, SourceVel);
		alSourcei(fuseSources.back(), AL_LOOPING, AL_TRUE);

		alSourcePlay(fuseSources.back());
	}
}

void SoundManager::updateFuses(GameState state) {
	if (initSuccess) {
		for (int i = 0; i < fuseSources.size(); i++) {
			if (i < state.numberOfPowerups()) {
				Powerup* bomb = state.getPowerup(i);

				ALfloat *SourcePos = vec3ToALfloat(bomb->getPos()).data();
				ALfloat *SourceVel = vec3ToALfloat(bomb->getVelocity()).data();

				alSourcef(fuseSources[i], AL_PITCH, 1.0f);
				float volume = distanceVolumeAdjuster(1.0f, bomb->getPos());
				alSourcef(fuseSources[i], AL_GAIN, volume);
				alSourcefv(fuseSources[i], AL_POSITION, SourcePos);
				alSourcefv(fuseSources[i], AL_VELOCITY, SourceVel);
				alSourcei(fuseSources[i], AL_LOOPING, AL_TRUE);
			}
		}
	}
}

void SoundManager::stopFuse(int fuseNum) {
	if (initSuccess) {
		if (fuseNum < fuseSources.size()) {
			alSourceStop(fuseSources[fuseNum]);

			alDeleteSources(1, &fuseSources[fuseNum]);
			fuseSources.erase(fuseSources.begin() + fuseNum);
			alDeleteBuffers(1, &fuseBuffers[fuseNum]);
			fuseBuffers.erase(fuseBuffers.begin() + fuseNum);
		}
	}
}


/*
 * Converts a vec3 to an array of ALfloats
 */
vector<ALfloat> SoundManager::vec3ToALfloat(vec3 vec) {
	vector<ALfloat> f = { vec.x, vec.y, vec.z };
	return f;
}

/*
 * Converts two vec3s to an array of ALfloats
 */
 vector<ALfloat> SoundManager::vec3ToALfloat(vec3 vector1, vec3 vector2) {
	vector<ALfloat> f = { vector1.x, vector1.y, vector1.z, vector2.x, vector2.y, vector2.z };
	return f;
}

/*
 * Loads a wav file to the given buffer and loads that buffer into the source
 * No need to add "Sounds\" to the file name. This method will do that
 * Just give "sound.wav"
 */
void SoundManager::loadWavToBuf(string fileName, ALuint* source, ALuint* buffer) {
	if (initSuccess) {
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

		delete[] buf;
		fclose(fp);
	}
}

/*
 * Loads and plays a bump sound once at the given position
 */
void SoundManager::playBumpSound(vec3 pos, float volume) {
	playSound("Bump.wav", pos, volume);
}

/*
* Loads and plays a bump sound once at the given position
*/
void SoundManager::playDingSound(vec3 pos) {
	playSound("Ding.wav", pos, 1.0);
}

void SoundManager::playSecretMusic(GameState state) {
	if (initSuccess) {
		alSourceStop(musicSource);
		ALuint buffer;
		int songSelection;

		do {
			songSelection = rand() % 5;
		} while (songSelection == lastSecretPlayed);
		lastSecretPlayed = songSelection;

		if (songSelection == 0) {
			loadWavToBuf("Secret.wav", &musicSource, &buffer);
		}
		else if (songSelection == 1) {
			loadWavToBuf("Dogstorm.wav", &musicSource, &buffer);
		}
		else if (songSelection == 2) {
			loadWavToBuf("Dogbass.wav", &musicSource, &buffer);
		}
		else if (songSelection == 3) {
			loadWavToBuf("DogLevels.wav", &musicSource, &buffer);
		}
		else {
			loadWavToBuf("DogsongMetal.wav", &musicSource, &buffer);
		}


		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcei(musicSource, AL_BUFFER, buffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(musicSource);
	}
}

void SoundManager::playPauseSong(GameState state) {
	if (initSuccess) {
		alSourceStop(musicSource);
		ALuint buffer;
		loadWavToBuf("PauseSong2.wav", &musicSource, &buffer);

		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcei(musicSource, AL_BUFFER, buffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(musicSource);
	}
}

void SoundManager::playPauseSong2(GameState state) {
	if (initSuccess) {
		alSourceStop(musicSource);
		ALuint buffer;
		loadWavToBuf("PauseSong3.wav", &musicSource, &buffer);

		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcei(musicSource, AL_BUFFER, buffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(musicSource);
	}
}

void SoundManager::updateMusicPitch(GameState state, Input input) {
	if (initSuccess) {
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
}

void SoundManager::playWinSound(vec3 pos) {
	if (initSuccess) {
		alSourcePause(musicSource);

		loadWavToBuf("Win.wav", &winSoundSource, &winSoundBuffer);

		ALfloat *SourcePos = vec3ToALfloat(pos).data();
		float volume = distanceVolumeAdjuster(1.0f, pos);

		alSourcei(winSoundSource, AL_BUFFER, winSoundBuffer);
		alSourcef(winSoundSource, AL_PITCH, 1.0f);
		alSourcef(winSoundSource, AL_GAIN, volume);
		alSourcefv(winSoundSource, AL_POSITION, SourcePos);
		alSourcei(winSoundSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(winSoundSource);

		gameOver = true;
	}
}

void SoundManager::playLossSound(vec3 pos) {
	if (initSuccess) {
		alSourcePause(musicSource);

		loadWavToBuf("Trombone.wav", &winSoundSource, &winSoundBuffer);

		ALfloat *SourcePos = vec3ToALfloat(pos).data();
		float volume = distanceVolumeAdjuster(1.0f, pos);

		alSourcei(winSoundSource, AL_BUFFER, winSoundBuffer);
		alSourcef(winSoundSource, AL_PITCH, 1.0f);
		alSourcef(winSoundSource, AL_GAIN, volume);
		alSourcefv(winSoundSource, AL_POSITION, SourcePos);
		alSourcei(winSoundSource, AL_LOOPING, AL_FALSE);

		alSourcePlay(winSoundSource);

		gameOver = true;
	}
}

void SoundManager::playMineExplosionSound(vec3 pos) {
	playSound("MineExplosion.wav", pos, 1.0f);
}

void SoundManager::playSound(string fileName, vec3 pos, float volume) {
	if (initSuccess) {
		ALuint source;
		ALuint buffer;

		loadWavToBuf(fileName, &source, &buffer);

		ALfloat *SourcePos = vec3ToALfloat(pos).data();
		volume = distanceVolumeAdjuster(volume, pos);

		alSourcei(source, AL_BUFFER, buffer);
		alSourcef(source, AL_PITCH, 1.0f);
		alSourcef(source, AL_GAIN, volume);
		alSourcefv(source, AL_POSITION, SourcePos);
		alSourcei(source, AL_LOOPING, AL_FALSE);

		alSourcePlay(source);
		oneTimeUseBuffers.push_back(buffer);
		oneTimeUseSources.push_back(source);
	}
}

void SoundManager::cleanOneTimeUseSources() {
	if (initSuccess) {
		for (int i = 0; i < oneTimeUseSources.size(); i++) {
			ALint soundState;
			alGetSourcei(oneTimeUseSources[i], AL_SOURCE_STATE, &soundState);
			if (soundState == AL_STOPPED) {
				alSourceStop(oneTimeUseSources[i]);


				alDeleteSources(1, &oneTimeUseSources[i]);
				oneTimeUseSources.erase(oneTimeUseSources.begin() + i);
				alDeleteBuffers(1, &oneTimeUseBuffers[i]);
				oneTimeUseBuffers.erase(oneTimeUseBuffers.begin() + i);

				// Only deletes one sound per frame
				// Very unlikely that more than one would run out in the same frame
				i = oneTimeUseSources.size();
			}
		}
	}
}

void SoundManager::playEndSong(GameState state) {
	if (initSuccess) {
		alSourceStop(musicSource);
		ALuint buffer;
		loadWavToBuf("EndGameSong.wav", &musicSource, &buffer);

		PlayerInfo* p1 = state.getPlayer(0);

		ALfloat *SourcePos = vec3ToALfloat(p1->getPos()).data();
		ALfloat *SourceVel = vec3ToALfloat(p1->getVelocity()).data();

		alSourcei(musicSource, AL_BUFFER, buffer);
		alSourcef(musicSource, AL_PITCH, 1.0f);
		alSourcef(musicSource, AL_GAIN, musicVolume);
		alSourcefv(musicSource, AL_POSITION, SourcePos);
		alSourcefv(musicSource, AL_VELOCITY, SourceVel);
		alSourcei(musicSource, AL_LOOPING, AL_TRUE);

		alSourcePlay(musicSource);
	}
}

/*
 * Pauses all sound effects, no effect on music
 * Win sound is also excluded BECAUSE NOTHING CAN STOP IT
 */
void SoundManager::pauseAllSounds() {
	if (initSuccess) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			alSourcePause(engineSources[i]);
			alSourcePause(honkSources[i]);
		}

		for (int i = 0; i < oneTimeUseSources.size(); i++) {
			alSourcePause(oneTimeUseSources[i]);
		}

		for (int i = 0; i < fuseSources.size(); i++) {
			alSourcePause(fuseSources[i]);
		}
	}
}

/*
 * Resumes all sound effects, no effect on music
 */
void SoundManager::resumeAllSounds() {
	if (initSuccess) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			alSourcePlay(engineSources[i]);
			alSourcePlay(honkSources[i]);
		}

		for (int i = 0; i < oneTimeUseSources.size(); i++) {
			alSourcePlay(oneTimeUseSources[i]);
		}

		for (int i = 0; i < fuseSources.size(); i++) {
			alSourcePlay(fuseSources[i]);
		}
	}
}

/*
 * Deletes ALL sources and buffers
 * Only to be used before restarting a game or closing the game
 */
void SoundManager::deleteAll() {
	if (initSuccess) {
		for (int i = 0; i < oneTimeUseSources.size(); i++) {
			alDeleteSources(1, &oneTimeUseSources[i]);
			oneTimeUseSources.erase(oneTimeUseSources.begin());
			alDeleteBuffers(1, &oneTimeUseBuffers[i]);
			oneTimeUseBuffers.erase(oneTimeUseBuffers.begin());
		}

		for (int i = 0; i < MAX_PLAYERS; i++) {
			alDeleteSources(1, &engineSources[i]);
			alDeleteBuffers(1, &engineBuffers[i]);

			alDeleteSources(1, &honkSources[i]);
			honkSources.erase(honkSources.begin());
			alDeleteBuffers(1, &honkBuffers[i]);
			honkBuffers.erase(honkBuffers.begin());
		}

		for (int i = 0; i < fuseSources.size(); i++) {
			stopFuse(i);
			i--;
		}

		alDeleteSources(1, &winSoundSource);
		alDeleteBuffers(1, &winSoundBuffer);

		alDeleteSources(1, &musicSource);
		alDeleteBuffers(1, &musicBuffer);

		paused = false;
		gameOver = false;
	}
}

void SoundManager::unpause(GameState state) {
	paused = false;
	resumeAllSounds();

	if (!secretPlaying) {
		alSourceStop(musicSource);
		alDeleteBuffers(1, &musicBuffer);
		alDeleteSources(1, &musicSource);
		startMusic(state);
	}
	pause2Playing = false;
	pauseSongPitch = 1.0f;
}

/*
 * Updates all sounds
 */
void SoundManager::updateSounds(GameState state, Input inputs[]) {
	if (initSuccess) {
		if (!paused && !gameOver) {
			updateListener(state);
			updateMusic(state);
			updateEngineSounds(state, inputs);
			updateFuses(state);
		}

		cleanOneTimeUseSources();

		if (inputs[0].konamiCode) {
			alDeleteBuffers(1, &musicBuffer);
			alDeleteSources(1, &musicSource);
			playSecretMusic(state);
			secretPlaying = true;
		}

		if (inputs[0].menu && !paused && !secretPlaying && !firstFrame) {
			paused = true;
			pauseAllSounds();

			if (!secretPlaying) {
				alDeleteBuffers(1, &musicBuffer);
				alDeleteSources(1, &musicSource);
				playPauseSong(state);
			}
		}
		else if (inputs[0].menu && paused) {
			paused = false;
			resumeAllSounds();

			if (!secretPlaying) {
				alSourceStop(musicSource);
				alDeleteBuffers(1, &musicBuffer);
				alDeleteSources(1, &musicSource);
				startMusic(state);
			}
			pause2Playing = false;
			pauseSongPitch = 1.0f;
		}

		if (paused) {
			if (!pause2Playing && pauseSongPitch < 2.0f) {
				pauseSongPitch += 0.00006f;
				alSourcef(musicSource, AL_PITCH, pauseSongPitch);
			}
		}

		if (gameOver) {
			ALint winSoundState;
			alGetSourcei(winSoundSource, AL_SOURCE_STATE, &winSoundState);
			if (winSoundState == AL_STOPPED) {
				alDeleteBuffers(1, &winSoundBuffer);
				alDeleteSources(1, &winSoundSource);
				playEndSong(state);
			}
		}

		ALint musicState;
		alGetSourcei(musicSource, AL_SOURCE_STATE, &musicState);
		if (musicState == AL_STOPPED) {
			alSourceStop(musicSource);
			secretPlaying = false;
			if (paused) {
				alDeleteBuffers(1, &musicBuffer);
				alDeleteSources(1, &musicSource);
				if (pauseSongPitch >= 2.0f) {
					playPauseSong2(state);
					pause2Playing = true;
				}
				else {
					playPauseSong(state);
				}
			}
			else {
				alDeleteBuffers(1, &musicBuffer);
				alDeleteSources(1, &musicSource);
				startMusic(state);
			}
		}

		for (int i = 0; i < state.getNbEvents(); i++) {
			Event* e = state.getEvent(i);

			if (e->getType() == VEHICLE_COLLISION_EVENT)
			{
				VehicleCollisionEvent* vehE = dynamic_cast<VehicleCollisionEvent*>(e);

				vec3 force = vehE->force;
				float volume = length(force);
				volume = map(volume, 0, 300000, 0, 2);
				cout << "Volume: " << volume << "\n";
				playBumpSound(vehE->location, volume);
			}
			else if (e->getType() == VEHICLE_WALL_COLLISION_EVENT) {
				VehicleWallCollisionEvent* vehE = dynamic_cast<VehicleWallCollisionEvent*>(e);

				vec3 force = vehE->force;
				float volume = length(force);
				volume = map(volume, 0, 300000, 0, 2);
				cout << "Volume: " << volume << "\n";
				playBumpSound(vehE->location, volume);
			}
			else if (e->getType() == GOLDEN_BUGGY_SWITCH_EVENT) {
				GoldenBuggySwitchEvent* gbE = dynamic_cast<GoldenBuggySwitchEvent*>(e);
				playSound("Boom.wav", gbE->gbPos, 1.0f);
			}
			else if (e->getType() == BOMB_CREATION_EVENT) {
				BombCreationEvent* bombE = dynamic_cast<BombCreationEvent*>(e);
				startFuse(state);
			}
			else if (e->getType() == VEHICLE_BOMB_COLLISION_EVENT) {
				VehicleBombCollisionEvent* boomE = dynamic_cast<VehicleBombCollisionEvent*>(e);

				playSound("LittleBoom.wav", boomE->location, 1.0f);
				stopFuse(boomE->ob2);
			}
		}
		// Feel free to remove this. It has no purpose, just sounds funny
		//updateMusicPitch(state, inputs[0]);

		if (firstFrame) {
			firstFrame = false;
		}
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
