#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

#include "player.h"

constexpr uint16_t SCR_HEIGHT = 720;
constexpr uint16_t SCR_WIDTH = 1280;

class RaycastingWindow
{
protected:
	//SDL2 things
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;

	//player of the game
	Player player;
	//player's mutex
	std::mutex playerMutex;

	//drawing functions
	void drawRaycaster();
	void drawHUD();
	//event handling functions
	void handleEvents(bool& ndone);
public:
	explicit RaycastingWindow();
	void mainLoop();
	~RaycastingWindow();

	RaycastingWindow(RaycastingWindow&) = delete;
};

#endif
