#ifndef SOUNDMANAGER_CPP
#define SOUNDMANAGER_CPP
#include "SoundManager.h"

Mix_Music *gMusic;
Mix_Chunk *idleSound;
bool idlePlaying;

SoundManager::SoundManager() {

}

SoundManager::SoundManager(GameState state) {
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

void SoundManager::updateSounds(GameState state) {
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
