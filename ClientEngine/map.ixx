export module map;

import window;

#include <SDL.h>
#include <cmath>
#include <algorithm>

export struct ColorRGB
{
	unsigned char r, g, b;
};

constexpr int mapWidth = 24;
constexpr int mapHeight = 24;

export inline constexpr int worldMap[mapWidth][mapHeight] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,0,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

export struct Player
{
	double posX, posY, posZ; // x, y, and z position of player when start
	double dirX, dirY; // direction when start
	double planeX, planeY; // 2d version of camera plane
	double pitch; // up or down

	bool wsad[8]; // check if button pressed

	bool jumping; //jumping?
	bool falling; //falling?

	double moveSpeed, rotSpeed; // used for the speed of the player
	double jumpMSpeed;
};

export void playerMovement(Player* p, double frameTime)
{
	double oldDirX;
	double oldPlaneX;

	//speed modifiers
	p->rotSpeed = frameTime * 2.0; //the constant value is in radians/second
	float addedSpeed = 0.0f;

	if (p->jumping)
		addedSpeed = 0.1f; // boost the speed when jumping
	if (p->wsad[4])//sprinting
		addedSpeed = p->jumping ? 0.115f : 7.11f;
	else if (!p->wsad[4] && !p->jumping)
		addedSpeed = 0.0f;

	//std::cout << addedSpeed << '\n';

	p->moveSpeed = frameTime * (3.0 + (double)(addedSpeed / 4)); //the constant value is in squares/second

	//could make more readable
	if (p->wsad[5] && !p->jumping && !p->falling)
		p->jumping = true;
	else if (p->jumping && p->posZ < 1200)
	{
		p->posZ += frameTime * (16 + p->jumpMSpeed);
		p->jumpMSpeed += 0.21;
	}
	else if (p->jumping && p->posZ > 1200)
	{
		p->jumping = false;
		p->falling = true;
	}
	if (p->falling && p->posZ > 0)
	{
		p->posZ -= frameTime * (28.5 + p->jumpMSpeed);
		p->jumpMSpeed += 0.05;
	}
	else if (p->falling && p->posZ < 0)
	{
		p->posZ = 0; // msets player to ground
		p->falling = false;
		p->jumping = false;
	}

	/*
	if (p->wsad[6])//look up
	{
		p->pitch += 5.0;
	}
	if (p->wsad[7])//look down
	{
		p->pitch -= 5.0;
	}
	*/

	if (p->wsad[0])//move forward
	{
		if (worldMap[int(p->posX + p->dirX * p->moveSpeed)][int(p->posY)] == false) p->posX += p->dirX * p->moveSpeed;
		if (worldMap[int(p->posX)][int(p->posY + p->dirY * p->moveSpeed)] == false) p->posY += p->dirY * p->moveSpeed;
	}
	if (p->wsad[1])//move backward
	{
		if (worldMap[int(p->posX - p->dirX * p->moveSpeed)][int(p->posY)] == false) p->posX -= p->dirX * p->moveSpeed;
		if (worldMap[int(p->posX)][int(p->posY - p->dirY * p->moveSpeed)] == false) p->posY -= p->dirY * p->moveSpeed;
	}
	if (p->wsad[2])//turn left
	{
		//both camera direction and camera plane must be rotated
		oldDirX = p->dirX;
		p->dirX = p->dirX * cos(-(p->rotSpeed)) - p->dirY * sin(-(p->rotSpeed));
		p->dirY = oldDirX * sin(-(p->rotSpeed)) + p->dirY * cos(-(p->rotSpeed));
		oldPlaneX = p->planeX;
		p->planeX = p->planeX * cos(-(p->rotSpeed)) - p->planeY * sin(-(p->rotSpeed));
		p->planeY = oldPlaneX * sin(-(p->rotSpeed)) + p->planeY * cos(-(p->rotSpeed));
	}
	if (p->wsad[3])//turn right
	{
		//both camera direction and camera plane must be rotated
		oldDirX = p->dirX;
		p->dirX = p->dirX * cos(p->rotSpeed) - p->dirY * sin(p->rotSpeed);
		p->dirY = oldDirX * sin(p->rotSpeed) + p->dirY * cos(p->rotSpeed);
		oldPlaneX = p->planeX;
		p->planeX = p->planeX * cos(-(p->rotSpeed)) - p->planeY * sin(p->rotSpeed);
		p->planeY = oldPlaneX * sin(p->rotSpeed) + p->planeY * cos(p->rotSpeed);
	}
}

export void drawLineRaycaster(SDL_Renderer* gRenderer, Player* p)
{
	for (int x = 0; x < SCR_WIDTH; x++)
	{
		double cameraX = 2 * double(x) / double(SCR_WIDTH) - 1; // x coordinate in cameraspace
		double rayDirX = p->dirX + p->planeX * cameraX;
		double rayDirY = p->dirY + p->planeY * cameraX;

		int mapX = int(p->posX);
		int mapY = int(p->posY);

		double sideDistX, sideDistY, perpWallDist, deltaDistX, deltaDistY;

		deltaDistX = std::abs(1 / rayDirX);
		deltaDistY = std::abs(1 / rayDirY);

		int stepX, stepY, hit = 0, side;

		//calculate step and initial sideDist
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (p->posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - p->posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (p->posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - p->posY) * deltaDistY;
		}

		//perform DDA
		while (hit == 0)
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			//Check if ray has hit a wall
			if (worldMap[mapX][mapY] > 0) hit = 1;
		}

		//Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
		if (side == 0) perpWallDist = (mapX - p->posX + (1 - stepX) / 2) / rayDirX;
		else           perpWallDist = (mapY - p->posY + (1 - stepY) / 2) / rayDirY;

		//Calculate height of line to draw on screen
		int lineHeight = (int)(SCR_WIDTH / perpWallDist);

		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = int((-lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist)) + int(p->pitch);
		if (drawStart < 0) drawStart = 0;

		int drawEnd = int((lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist)) + int(p->pitch);
		if (drawEnd >= SCR_HEIGHT) drawEnd = SCR_HEIGHT - 1;

		//choose wall color
		ColorRGB color;
		switch (worldMap[mapX][mapY])
		{
		case 1://red
			color = { 255,0,0 };
			break;
		case 2://green
			color = { 0,255,0 };
			break;
		case 3://blue
			color = { 0,0,255 };
			break;
		case 4://white
			color = { 255,255,255 };
			break;
		default://yellow
			color = { 255,255,0 };
			break;
		}

		//give x and y sides different brightness
		if (side == 1)
		{
			color.r = color.r / 2;
			color.g = color.g / 2;
			color.b = color.b / 2;
		}
		//shading
		color.r = (unsigned char)std::clamp((int)(color.r) - (int)(perpWallDist * 3), 0, 255);
		color.g = (unsigned char)std::clamp((int)(color.g) - (int)(perpWallDist * 3), 0, 255);
		color.b = (unsigned char)std::clamp((int)(color.b) - (int)(perpWallDist * 3), 0, 255);

		//shade the floor to look like reflections
		const auto darkGrey = std::clamp(side == 1 ? (100 - (int)(perpWallDist * 3)) / 2 : 100 - (int)(perpWallDist * 3), 0, 255);
		//shade the ceiling to look nice
		const auto lightGrey = std::clamp(side == 1 ? (200 - (int)(perpWallDist / 4)) / 2 : 200 - (int)(perpWallDist / 4), 0, 255);

		//draw the pixels of the stripe as a vertical linefor the ceiling
		SDL_SetRenderDrawColor(gRenderer, lightGrey, lightGrey, lightGrey, 255);
		SDL_RenderDrawLine(gRenderer, x, 0, x, drawStart);

		//draw the pixels of the stripe as a vertical linefor floor
		SDL_SetRenderDrawColor(gRenderer, darkGrey, darkGrey, darkGrey, 255);
		SDL_RenderDrawLine(gRenderer, x, drawEnd, x, SCR_HEIGHT);

		//draw the pixels of the stripe as a vertical line for the walls
		SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, 255);
		SDL_RenderDrawLine(gRenderer, x, drawStart, x, drawEnd);
	}
}

export void drawHUD(SDL_Renderer* gRenderer, const int w, const int h)
{
	//draw hud
	SDL_SetRenderDrawColor(gRenderer, 220, 220, 220, 230);
	const int topleftx = h / 8;
	const SDL_Rect hudBox = { 0, h - topleftx, w, topleftx };
	SDL_RenderDrawRect(gRenderer, &hudBox);
	SDL_RenderFillRect(gRenderer, &hudBox);
}