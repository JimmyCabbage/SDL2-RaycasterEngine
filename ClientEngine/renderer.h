#ifndef RAYCASTER_RENDERERER_H
#define RAYCASTER_RENDERERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"
#include "screen.h"

class RaycasterRenderer
{
protected:
	SDL_Renderer* gRenderer;

	//SDL2 TTF things
	TTF_Font* Sans;

	//drawing functions
	void drawRaycaster(Player& player);
	void drawHUD(Player& p);
	void drawPlayerPosition(Player& player);
public:
	void draw(Player& p);

	RaycasterRenderer(SDL_Window*& window);

	~RaycasterRenderer();
};

#endif