#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

#include "player.h"
#include "renderer.h"

class RaycastingWindow
{
protected:
	//SDL2 things
	SDL_Window* gWindow;

	RaycasterRenderer* gRenderer;

	//player of the game
	Player player;
	//player's mutex
	std::mutex playerMutex;

	//event handling functions
	void handleEvents(bool& ndone);
public:
	explicit RaycastingWindow();
	void mainLoop();
	~RaycastingWindow();

	RaycastingWindow(RaycastingWindow&) = delete;
};

#endif
