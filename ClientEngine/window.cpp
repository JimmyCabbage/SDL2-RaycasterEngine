#include "window.h"
#include "map.h"
#include "screen.h"

#include <future>
#include <cmath>
#include <algorithm>
#include <exception>
#include <string>

RaycastingWindow::RaycastingWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::runtime_error(std::string("Could not initialize SDL: ") + SDL_GetError());
	}

	gWindow = nullptr;
	gWindow = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (!gWindow)
	{
		throw std::runtime_error(std::string("Could not create SDL window: ") + SDL_GetError());
	}

	gRenderer = nullptr;
	gRenderer = new RaycasterRenderer(gWindow);

	// initialize player
	player = Player();

	player.posX = 22, player.posY = 12, player.posZ = 0; // setup player x, y, and z variables
	player.dirX = -1, player.dirY = 0; // direction when start
	player.planeX = 0, player.planeY = 0.76; // 2d version of camera plane
}

void RaycastingWindow::handleEvents(bool& ndone)
{
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
			case 'a':
			{
				std::lock_guard lock(playerMutex);
				player.wsad[8] = ev.type == SDL_KEYDOWN;
			}
			break;
			case 'd':
			{
				std::lock_guard lock(playerMutex);
				player.wsad[9] = ev.type == SDL_KEYDOWN;
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
			default: break;
			}
			break;
		default:
			break;
		}
	}
}

void RaycastingWindow::mainLoop()
{
	//time handling
	uint32_t oldTime = 0; // time of the previous frame
	double frameTime;

	//void (Player:: * movement)(std::mutex&, double, double) = &Player::Move;

	bool ndone = true;
	while (ndone)
	{
		//timing for input and FPS counter
		uint32_t time = SDL_GetTicks(); //time of current frame
		frameTime = ((time - oldTime) / 1000.0); //frameTime is the time this frame has taken, in seconds
		oldTime = time;

		std::thread playerMovementThread = std::thread(&Player::Move, &player, std::ref(playerMutex), frameTime, (time / 1000.0));

		//?? IS THIS THREAD SAFE?
		gRenderer->draw(player);

		handleEvents(ndone);
		
		if (playerMovementThread.joinable())
			playerMovementThread.join();
	}
}

RaycastingWindow::~RaycastingWindow()
{
	delete gRenderer;

	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	SDL_Quit();
}