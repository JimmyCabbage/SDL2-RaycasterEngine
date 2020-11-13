#include "player.h"
#include "map.h"

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

	jumping = p.jumping;
	falling = p.falling;

	moveSpeed = p.moveSpeed;
	rotSpeed = p.rotSpeed;

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

	jumping = 0.0;
	falling = 0.0;

	moveSpeed = 0.0;
	rotSpeed = 0.0;

	jumpMSpeed = 0.0;
}

void playerMovement(Player* p, std::mutex& playermut, double frameTime)
{
	double oldDirX;
	double oldPlaneX;

	playermut.lock();
	Player pCopy(*p);
	playermut.unlock();

	//speed modifiers
	pCopy.rotSpeed = frameTime * 2.0; //the constant value is in radians/second
	float addedSpeed = 0.0f;

	if (pCopy.jumping)
		addedSpeed = pCopy.wsad[4] ? 0.115f : 0.1f; // boost the speed when jumping
	else if (pCopy.wsad[4])//sprinting
		addedSpeed = 7.11f;
	else if (!pCopy.wsad[4] && !pCopy.jumping)
		addedSpeed = 0.0f;
	
	pCopy.moveSpeed = frameTime * (3.0 + (double)(addedSpeed / 4)); //the constant value is in squares/second

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

	/*
	if (pCopy.wsad[6])//look up
	{
		pCopy.pitch += 5.0;
	}
	if (pCopy.wsad[7])//look down
	{
		pCopy.pitch -= 5.0;
	}
	*/

	if (pCopy.wsad[0])//move forward
	{
		if (worldMap[int(pCopy.posX + pCopy.dirX * pCopy.moveSpeed)][int(pCopy.posY)] == false) pCopy.posX += pCopy.dirX * pCopy.moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY + pCopy.dirY * pCopy.moveSpeed)] == false) pCopy.posY += pCopy.dirY * pCopy.moveSpeed;
	}
	if (pCopy.wsad[1])//move backward
	{
		if (worldMap[int(pCopy.posX - pCopy.dirX * pCopy.moveSpeed)][int(pCopy.posY)] == false) pCopy.posX -= pCopy.dirX * pCopy.moveSpeed;
		if (worldMap[int(pCopy.posX)][int(pCopy.posY - pCopy.dirY * pCopy.moveSpeed)] == false) pCopy.posY -= pCopy.dirY * pCopy.moveSpeed;
	}
	if (pCopy.wsad[2])//turn left
	{
		//both camera direction and camera plane must be rotated
		oldDirX = pCopy.dirX;
		pCopy.dirX = pCopy.dirX * cos(-(pCopy.rotSpeed)) - pCopy.dirY * sin(-(pCopy.rotSpeed));
		pCopy.dirY = oldDirX * sin(-(pCopy.rotSpeed)) + pCopy.dirY * cos(-(pCopy.rotSpeed));
		oldPlaneX = pCopy.planeX;
		pCopy.planeX = pCopy.planeX * cos(-(pCopy.rotSpeed)) - pCopy.planeY * sin(-(pCopy.rotSpeed));
		pCopy.planeY = oldPlaneX * sin(-(pCopy.rotSpeed)) + pCopy.planeY * cos(-(pCopy.rotSpeed));
	}
	if (pCopy.wsad[3])//turn right
	{
		//both camera direction and camera plane must be rotated
		oldDirX = pCopy.dirX;
		pCopy.dirX = pCopy.dirX * cos(pCopy.rotSpeed) - pCopy.dirY * sin(pCopy.rotSpeed);
		pCopy.dirY = oldDirX * sin(pCopy.rotSpeed) + pCopy.dirY * cos(pCopy.rotSpeed);
		oldPlaneX = pCopy.planeX;
		pCopy.planeX = pCopy.planeX * cos(-(pCopy.rotSpeed)) - pCopy.planeY * sin(pCopy.rotSpeed);
		pCopy.planeY = oldPlaneX * sin(pCopy.rotSpeed) + pCopy.planeY * cos(pCopy.rotSpeed);
	}

	playermut.lock();
	*p = Player(pCopy);
	playermut.unlock();
}