#include <iostream>
#include <cmath>
#include <future>
#include <SDL.h>

#include "window.h"
#include "map.h"
#include "player.h"

//#define MULTITHREADED

int main(int argc, char** argv)
{
	double frameTime;

	double time = 0; //time of current frame
	double oldTime = 0; // time of the previous frame

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "Could not initiate SDL: " << SDL_GetError() << '\n';
		exit(1);
	}

	SDL_Window* gWindow = nullptr;
	gWindow = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!gWindow)
	{
		std::cerr << "Unable to create SDL window: " << SDL_GetError() << '\n';
		exit(1);
	}

	SDL_Renderer* gRenderer = nullptr;
	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	if (!gRenderer)
	{
		std::cerr << "Unable to create SDL renderer: " << SDL_GetError() << '\n';
		exit(1);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(gRenderer, SCR_WIDTH, SCR_HEIGHT);

	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	// initialize player variables
	Player player;
	player.jumping = false;
	player.falling = false;

	player.pitch = 0;
	player.wsad[0] = false;
	player.wsad[1] = false;
	player.wsad[2] = false;
	player.wsad[3] = false;
	player.wsad[4] = false;
	player.wsad[5] = false;
	player.wsad[6] = false;
	player.wsad[7] = false;

	player.jumpMSpeed = 0.0;

	player.posX = 22, player.posY = 12, player.posZ = 0; // setup player x, y, and z variables
	player.dirX = -1, player.dirY = 0; // direction when start
	player.planeX = 0, player.planeY = 0.66; // 2d version of camera plane

	bool ndone = true;
	while (ndone)
	{
		drawLineRaycaster(gRenderer, &player);

		drawHUD(gRenderer, SCR_WIDTH, SCR_HEIGHT);

		SDL_RenderPresent(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);

		//timing for input and FPS counter
		oldTime = time;
		time = SDL_GetTicks();
		frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
		
		std::mutex playerMutex;
		std::thread playerMovementThread = std::thread(&playerMovement, &player, std::ref(playerMutex), frameTime);

		SDL_Event ev;

		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				ndone = false;
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch (ev.key.keysym.sym)
				{
				case 'w':
				{
					std::lock_guard lock(playerMutex);
					player.wsad[0] = ev.type == SDL_KEYDOWN;
				}
					break;
				case 's':
				{
					std::lock_guard lock(playerMutex);
					player.wsad[1] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_LEFT:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[3] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_RIGHT:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[2] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_LSHIFT:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[4] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_SPACE:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[5] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_UP:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[6] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_DOWN:
				{
					std::lock_guard lock(playerMutex);
					player.wsad[7] = ev.type == SDL_KEYDOWN;
				}
					break;
				case SDLK_ESCAPE:
					ndone = false;
					break;
				case 'q': ndone = false; break;
				default: break;
				}
				break;
			default:
				break;
			}
		}

		if (playerMovementThread.joinable())
			playerMovementThread.join();
	}

	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_Quit();
	return 0;
}