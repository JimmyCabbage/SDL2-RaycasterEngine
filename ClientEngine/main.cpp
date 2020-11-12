#include <cstdlib>
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
#ifdef MULTITHREADED
		auto renderDraw = std::async(std::launch::async, drawLineRaycaster, &gRenderer, &player);
#else
		drawLineRaycaster(gRenderer, &player);

		drawHUD(gRenderer, SCR_WIDTH, SCR_HEIGHT);

		SDL_RenderPresent(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);
#endif

		//timing for input and FPS counter
		oldTime = time;
		time = SDL_GetTicks();
		frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				ndone = false;
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case 'w':
					player.wsad[0] = event.type == SDL_KEYDOWN;
					break;
				case 's':
					player.wsad[1] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_LEFT:
					player.wsad[3] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_RIGHT:
					player.wsad[2] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_LSHIFT:
					player.wsad[4] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_SPACE:
					player.wsad[5] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_UP:
					player.wsad[6] = event.type == SDL_KEYDOWN;
					break;
				case SDLK_DOWN:
					player.wsad[7] = event.type == SDL_KEYDOWN;
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

		playerMovement(&player, frameTime);
		
#ifdef MULTITHREADED
		renderDraw.get();
		drawHUD(&gRenderer, SCR_WIDTH, SCR_HEIGHT);

		SDL_RenderPresent(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);
#endif
	}

	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;

	SDL_Quit();
	return 0;
}