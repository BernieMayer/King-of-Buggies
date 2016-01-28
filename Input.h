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
};

#endif //INPUT_H