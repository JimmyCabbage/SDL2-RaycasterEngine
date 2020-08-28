#ifndef PLAYER_H
#define PLAYER_H
#include "map.h"

struct Player
{
	double posX, posY, posZ; // x, y, and z position of player when start
	double dirX, dirY; // direction when start
	double planeX, planeY; // 2d version of camera plane
	double pitch; // up or down

	int wsad[8]; // check if button pressed

	bool jumping; //jumping?
	bool falling; //falling?

	double moveSpeed, rotSpeed; // used for the speed of the player
	double jumpMSpeed;
};

void playerMovement(struct Player* p, double frameTime);


#endif