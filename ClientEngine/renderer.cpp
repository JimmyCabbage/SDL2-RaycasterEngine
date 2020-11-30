#include "renderer.h"

#include <cmath>
#include <algorithm>
#include <string>
#include "map.h"

/*
 * 0 limits the colors
 * 1 adds more colors
 * 2 limits the colors a lot
 * NOTE: Should (probably) always have this enabled to 1 soon, instead of a switch. But computing colors may take more time
 */
#define EXTRACOLOR 0

 /*
  * 0 makes the renderer add shading to walls
  * 1 makes everything fully lit
  * NOTE:
  *
  */
#define FULLBRIGHT 0

struct ColorRGB
{
	uint8_t r, g, b;
};

void RaycasterRenderer::drawRaycaster(Player& player)
{
	for (int x = 0; x < SCR_WIDTH; x++)
	{
		double cameraX = 2 * double(x) / double(SCR_WIDTH) - 1; // x coordinate in cameraspace
		double rayDirX = player.dirX + player.planeX * cameraX;
		double rayDirY = player.dirY + player.planeY * cameraX;

		int mapX = int(player.posX);
		int mapY = int(player.posY);

		double sideDistX, sideDistY, perpWallDist, deltaDistX, deltaDistY;

		deltaDistX = std::abs(1 / rayDirX);
		deltaDistY = std::abs(1 / rayDirY);

		int stepX, stepY, hit = 0, side = 0;

		//calculate step and initial sideDist
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (player.posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - player.posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (player.posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - player.posY) * deltaDistY;
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
		if (side == 0) perpWallDist = (mapX - player.posX + (1 - stepX) / 2) / rayDirX;
		else           perpWallDist = (mapY - player.posY + (1 - stepY) / 2) / rayDirY;

		//Calculate height of line to draw on screen
		int lineHeight = (int)(SCR_WIDTH / perpWallDist);

		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = (int)((-lineHeight / 2 + SCR_HEIGHT / 2) + (player.posZ / perpWallDist)) + (int)(player.pitch);
		if (drawStart < 0) drawStart = 0;

		int drawEnd = (int)((lineHeight / 2 + SCR_HEIGHT / 2) + (player.posZ / perpWallDist)) + (int)(player.pitch);
		if (drawEnd >= SCR_HEIGHT) drawEnd = SCR_HEIGHT - 1;

		//choose wall color
		ColorRGB color;
		switch (worldMap[mapX][mapY])
		{
		case 1://red
			color.r = 255;
			color.g = 0;
			color.b = 0;
			break;
		case 2://green
			color.r = 0;
			color.g = 255;
			color.b = 0;
			break;
		case 3://blue
			color.r = 0;
			color.g = 0;
			color.b = 255;
			break;
		case 4://white
			color.r = 255;
			color.g = 255;
			color.b = 255;
			break;
#if EXTRACOLOR
		case 5://pink
			color.r = 230;
			color.g = 192;
			color.b = 203;
			break;
		case 6://cyan
			color.r = 92;
			color.g = 249;
			color.b = 255;
			break;
		case 7://dark magenta
			color.r = 127;
			color.g = 0;
			color.b = 153;
			break;
		case 8://amber
			color.r = 255;
			color.g = 126;
			color.b = 0;
			break;
#endif
		default://yellow
			color.r = 255;
			color.g = 255;
			color.b = 0;
			break;
		}

#if (!(FULLBRIGHT) && (EXTRACOLOR == 1 || EXTRACOLOR == 0))
		//give x and y sides different brightness
		if (side == 1)
		{
			color.r = color.r / 2;
			color.g = color.g / 2;
			color.b = color.b / 2;
		}
		//shading
		//NOTE: Calculate color in int range, then cast to uint8_t

		color.r = (uint8_t)std::clamp(color.r - (int)(perpWallDist * 3), 0, 255);
		color.g = (uint8_t)std::clamp(color.g - (int)(perpWallDist * 3), 0, 255);
		color.b = (uint8_t)std::clamp(color.b - (int)(perpWallDist * 3), 0, 255);

		//shade the floor to look like reflections
		const uint8_t darkGrey = (uint8_t)std::clamp(side == 1 ? (100 - (int)(perpWallDist * 3)) / 2 : 100 - (int)(perpWallDist * 3), 0, 255);
		//shade the ceiling to look nice
		const uint8_t lightGrey = (uint8_t)std::clamp(side == 1 ? (200 - (int)(perpWallDist / 4)) / 2 : 200 - (int)(perpWallDist / 4), 0, 255);
#elif (!(FULLBRIGHT) && EXTRACOLOR == 2)
		//give x and y sides different brightness
		if (side == 1)
		{
			color.r = color.r / 2;
			color.g = color.g / 2;
			color.b = color.b / 2;
		}
		//shading
		//NOTE: This cheaper style calculates color in uint8_t range, which could cause underflow or overflow

		color.r = (uint8_t)(color.r - (int)(perpWallDist * 3));
		color.g = (uint8_t)(color.g - (int)(perpWallDist * 3));
		color.b = (uint8_t)(color.b - (int)(perpWallDist * 3));

		//shade the floor to look like reflections
		const uint8_t darkGrey = (uint8_t)(side == 1 ? (100 - (int)(perpWallDist * 3)) / 2 : (100 - (int)(perpWallDist * 3)));
		//shade the ceiling to look nice
		const uint8_t lightGrey = (uint8_t)(side == 1 ? (200 - (int)(perpWallDist / 4)) / 2 : (200 - (int)(perpWallDist / 4)));
#else
		constexpr uint8_t darkGrey = 100;
		constexpr uint8_t lightGrey = 100;
#endif

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

constexpr int topleftx = SCR_HEIGHT / 8;

void RaycasterRenderer::drawHUD(Player& p)
{
	constexpr SDL_Rect hudBox = { 0, SCR_HEIGHT - topleftx, SCR_WIDTH, topleftx };

	//draw hud background
	SDL_SetRenderDrawColor(gRenderer, 220, 220, 220, 200);
	SDL_RenderDrawRect(gRenderer, &hudBox);
	SDL_RenderFillRect(gRenderer, &hudBox);

	drawPlayerPosition(p);
}

void RaycasterRenderer::drawPlayerPosition(Player& player)
{
	constexpr SDL_Rect xposBox = { 0, SCR_HEIGHT - topleftx, SCR_WIDTH / 4, topleftx };
	constexpr SDL_Rect yposBox = { SCR_WIDTH / 4 + SCR_WIDTH / 8, SCR_HEIGHT - topleftx, SCR_WIDTH / 4, topleftx };
	constexpr SDL_Rect pitchBox = { SCR_WIDTH / 2 + SCR_WIDTH / 8 + SCR_WIDTH / 8, SCR_HEIGHT - topleftx, SCR_WIDTH / 4, topleftx };
	constexpr SDL_Color Grey = { 125, 125, 125 };

	SDL_Surface* xposText = TTF_RenderText_Solid(Sans, std::to_string(player.posX).c_str(), Grey);
	SDL_Surface* yposText = TTF_RenderText_Solid(Sans, std::to_string(player.posY).c_str(), Grey);
	SDL_Surface* pitchText = TTF_RenderText_Solid(Sans, std::to_string(player.pitch).c_str(), Grey);

	SDL_Texture* xposTexture = SDL_CreateTextureFromSurface(gRenderer, xposText);
	SDL_Texture* yposTexture = SDL_CreateTextureFromSurface(gRenderer, yposText);
	SDL_Texture* pitchTexture = SDL_CreateTextureFromSurface(gRenderer, pitchText);

	SDL_FreeSurface(xposText);
	SDL_FreeSurface(yposText);
	SDL_FreeSurface(pitchText);

	SDL_RenderCopy(gRenderer, xposTexture, nullptr, &xposBox);
	SDL_RenderCopy(gRenderer, yposTexture, nullptr, &yposBox);
	SDL_RenderCopy(gRenderer, pitchTexture, nullptr, &pitchBox);

	SDL_DestroyTexture(xposTexture);
	SDL_DestroyTexture(yposTexture);
	SDL_DestroyTexture(pitchTexture);
}

void RaycasterRenderer::draw(Player& p)
{
	drawRaycaster(p);

	drawHUD(p);

	SDL_RenderPresent(gRenderer);
	//we don't need this because the raycaster is already supposed to fill the whole screen
#if 0
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
#endif
}

RaycasterRenderer::RaycasterRenderer(SDL_Window*& window)
{
	if (TTF_Init() < 0)
	{
		throw std::runtime_error("Could not initialize SDL2 TTF");
	}

	gRenderer = nullptr;
	gRenderer = SDL_CreateRenderer(window, -1, 0);
	if (!gRenderer)
	{
		throw std::runtime_error(std::string("Unable to create SDL renderer: ") + SDL_GetError());
	}

	//set window and render data
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(gRenderer, SCR_WIDTH, SCR_HEIGHT);

	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	Sans = TTF_OpenFont("sans.ttf", 24);
}

RaycasterRenderer::~RaycasterRenderer()
{
	TTF_Quit();

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
}