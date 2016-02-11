#ifndef INPUT_H
#define INPUT_H

struct Input
{
	float forward;
	float turn;
	float camV;
	float camH;

	bool drift;
	bool powerup;
	bool menu;

	bool isKeyboard;
};

#endif //INPUT_H