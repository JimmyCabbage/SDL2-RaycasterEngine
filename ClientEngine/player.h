#ifndef PLAYER_H
#define PLAYER_H

#include <mutex>

class Player
{
public:
	double posX, posY, posZ; // x, y, and z position of player when start
	double dirX, dirY; // direction when start
	double planeX, planeY; // 2d version of camera plane
	double pitch; // up or down

	bool wsad[10]; // check if button pressed

	bool jumping; //jumping?
	bool falling; //falling?

	double jumpMSpeed;

	Player(Player& p);
	Player();

	void Move(std::mutex& playermut, double frameTime, double time);
};

#endif