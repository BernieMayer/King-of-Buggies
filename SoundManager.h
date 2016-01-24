#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

class SoundManager {
public:
	void playSound(FILE, vec3, float);
	void adjustPitch(int, float);
	void adjustVolume(int, float);
	void stopSound(int);
};

#endif // SOUNDMANAGER_H