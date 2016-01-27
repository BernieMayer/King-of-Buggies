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

	int playerNum;
};

#endif //INPUT_H