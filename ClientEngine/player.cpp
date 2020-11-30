#include "player.h"
#include "map.h"

#include <algorithm>

/*
 * 0 for collision checking
 * 1 for no collision with level geometry
 * NOTE: VERY DANGEROUS. Undefined behavior if set to 1. Use at your own risk.
 * I take no responsibilities if this... let's say bricks your computer.
 * YOU HAVE BEEN WARNED
 */
#define NCLIP 0

Player::Player(Player& p)
{
	posX = p.posX; posY = p.posY; posZ = p.posZ;
	dirX = p.dirX; dirY = p.dirY;
	planeX = p.planeX; planeY = p.planeY;
	pitch = p.pitch;

	wsad[0] = p.wsad[0];
	wsad[1] = p.wsad[1];
	wsad[2] = p.wsad[2];
	wsad[3] = p.wsad[3];
	wsad[4] = p.wsad[4];
	wsad[5] = p.wsad[5];
	wsad[6] = p.wsad[6];
	wsad[7] = p.wsad[7];
	wsad[8] = p.wsad[8];
	wsad[9] = p.wsad[9];

	jumping = p.jumping;
	falling = p.falling;

	jumpMSpeed = p.jumpMSpeed;
}

Player::Player()
{
	posX = 0.0; posY = 0.0; posZ = 0.0;
	dirX = 0.0; dirY = 0.0;
	planeX = 0.0; planeY = 0.0;
	pitch = 0.0;

	wsad[0] = false;
	wsad[1] = false;
	wsad[2] = false;
	wsad[3] = false;
	wsad[4] = false;
	wsad[5] = false;
	wsad[6] = false;
	wsad[7] = false;
	wsad[8] = false;
	wsad[9] = false;

	jumping = 0.0;
	falling = 0.0;

	jumpMSpeed = 0.0;
}

void Player::Move(std::mutex& playermut, double frameTime, double time)
{
	/*
	 * TODO:	Make this single-threaded or improve multithreading,
	 *			because this way of copying the variable directly
	 *			can lead to a condition where this thread is made,
	 *			event loop is called and updates the player class to handle inputs,
	 *			but this function ends and the inputs are overwritten with the old ones
	 */
	playermut.lock();
	Player pCopy(*this);
	playermut.unlock();

	//speed modifiers
	double rotSpeed = frameTime * 2.0; //the constant value is in radians/second
	float addedSpeed = 0.0f;

	if (pCopy.jumping)
		addedSpeed = pCopy.wsad[4] ? 0.115f : 0.1f; // boost the speed when jumping
	else if (pCopy.wsad[4])//sprinting
		addedSpeed = 7.11f;
	else if (!pCopy.wsad[4] && !pCopy.jumping)
		addedSpeed = 0.0f;
	
	double moveSpeed = frameTime * (3.0 + (double)(addedSpeed / 4)); //the constant value is in squares/second

	if (pCopy.wsad[5] && !pCopy.jumping && !pCopy.falling)
		pCopy.jumping = true;
	else if (pCopy.jumping && pCopy.posZ < 1200)
	{
		pCopy.posZ += frameTime * (16 + pCopy.jumpMSpeed);
		pCopy.jumpMSpeed += 0.21;
	}
	else if (pCopy.jumping && pCopy.posZ > 1200)
	{
		pCopy.jumping = false;
		pCopy.falling = true;
	}
	else if (pCopy.falling && pCopy.posZ > 0)
	{
		pCopy.posZ -= frameTime * (28.5 + pCopy.jumpMSpeed);
		pCopy.jumpMSpeed += 0.05;
	}
	else if (pCopy.falling && pCopy.posZ < 0)
	{
		pCopy.posZ = 0; // msets player to ground
		pCopy.falling = false;
		pCopy.jumping = false;
	}

	if (pCopy.wsad[6])//look up
	{
		pCopy.pitch += 2.0;
		pCopy.pitch = std::clamp(pCopy.pitch, -125.0, 125.0);
	}
	if (pCopy.wsad[7])//look down
	{
		pCopy.pitch -= 2.0;
		pCopy.pitch = std::clamp(pCopy.pitch, -125.0, 125.0);
	}

	if (pCopy.wsad[0])//move forward
	{
#if NCLIP
		pCopy.posX += pCopy.dirX * moveSpeed;
		pCopy.posY += pCopy.dirY * moveSpeed;
#else
		if (worldMap[int(pCopy.posX + pCopy.dirX * moveSpeed)][int(pCopy.posY)] == false) pCopy.posX += pCopy.dirX * moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY + pCopy.dirY * moveSpeed)] == false) pCopy.posY += pCopy.dirY * moveSpeed;
#endif
	}
	if (pCopy.wsad[1])//move backward
	{
#if NCLIP
		pCopy.posX -= pCopy.dirX * moveSpeed;
		pCopy.posY -= pCopy.dirY * moveSpeed;
#else
		if (worldMap[int(pCopy.posX - pCopy.dirX * moveSpeed)][int(pCopy.posY)] == false) pCopy.posX -= pCopy.dirX * moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY - pCopy.dirY * moveSpeed)] == false) pCopy.posY -= pCopy.dirY * moveSpeed;
#endif
	}
	if (pCopy.wsad[2])//turn left
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = pCopy.dirX;
		pCopy.dirX = pCopy.dirX * cos(-(rotSpeed)) - pCopy.dirY * sin(-(rotSpeed));
		pCopy.dirY = oldDirX * sin(-(rotSpeed)) + pCopy.dirY * cos(-(rotSpeed));
		double oldPlaneX = pCopy.planeX;
		pCopy.planeX = pCopy.planeX * cos(-(rotSpeed)) - pCopy.planeY * sin(-(rotSpeed));
		pCopy.planeY = oldPlaneX * sin(-(rotSpeed)) + pCopy.planeY * cos(-(rotSpeed));
	}
	if (pCopy.wsad[3])//turn right
	{
		//both camera direction and camera plane must be rotated
		double oldDirX = pCopy.dirX;
		pCopy.dirX = pCopy.dirX * cos(rotSpeed) - pCopy.dirY * sin(rotSpeed);
		pCopy.dirY = oldDirX * sin(rotSpeed) + pCopy.dirY * cos(rotSpeed);
		double oldPlaneX = pCopy.planeX;
		pCopy.planeX = pCopy.planeX * cos(-(rotSpeed)) - pCopy.planeY * sin(rotSpeed);
		pCopy.planeY = oldPlaneX * sin(rotSpeed) + pCopy.planeY * cos(rotSpeed);
	}
	if (pCopy.wsad[8])//strafe left
	{
#if NCLIP
		pCopy.posX += -pCopy.dirY * moveSpeed;
		pCopy.posY +=  pCopy.dirX * moveSpeed;
#else
		//use crazy magic to rotate vector 90 degrees
		//NOTE: THIS IS MY 3rd TIME IMPLEMENTING THIS? WHY DOES IT GO AWAY? DO I FORGET TO COMMIT? WHAT?
		double nDirX = -pCopy.dirY;
		double nDirY = pCopy.dirX;
		if (worldMap[int(pCopy.posX + nDirX * moveSpeed)][int(pCopy.posY)] == false) pCopy.posX += nDirX * moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY + nDirY * moveSpeed)] == false) pCopy.posY += nDirY * moveSpeed;
#endif
	}
	if (pCopy.wsad[9])//strafe right
	{
#if NCLIP
		pCopy.posX +=  pCopy.dirY * moveSpeed;
		pCopy.posY += -pCopy.dirX * moveSpeed;
#else
		//use crazy magic to rotate vector -90 degrees
		double nDirX = pCopy.dirY;
		double nDirY = -pCopy.dirX;
		if (worldMap[int(pCopy.posX + nDirX * moveSpeed)][int(pCopy.posY)] == false) pCopy.posX += nDirX * moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY + nDirY * moveSpeed)] == false) pCopy.posY += nDirY * moveSpeed;
#endif
	}

	playermut.lock();
	*this = Player(pCopy);
	playermut.unlock();
}