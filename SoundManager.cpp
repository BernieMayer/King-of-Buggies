#ifndef SOUNDMANAGER_CPP
#define SOUNDMANAGER_CPP
#include "SoundManager.h"

Mix_Music *gMusic;

SoundManager::SoundManager() {
	gMusic = NULL;
	bool success = true;

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
}

bool SoundManager::loadMedia() {
	bool success = true;

	// Load music
	gMusic = Mix_LoadMUS("Sounds/Dogsong.wav");
	if (gMusic == NULL) {
		cout << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << "\n";
		success = false;
	}

	if (success) {
		// Number is number of times music is repeated
		// -1 means loop
		Mix_PlayMusic(gMusic, -1);
	}

	return success;
}

#endif
