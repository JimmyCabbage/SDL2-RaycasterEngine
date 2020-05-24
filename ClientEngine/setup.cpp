#include "setup.h"

void setupSDL2()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "Could not initiate SDL: " << SDL_GetError() << '\n';
        exit(1);
    }

    gWindow = SDL_CreateWindow("Raycaster in SDL2 Port", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_SHOWN);

    if (!gWindow)
    {
        std::cerr << "Unable to create SDL window: " << SDL_GetError() << '\n';
        exit(1);
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);

    if (!gRenderer)
    {
        std::cerr << "Unable to create SDL renderer: " << SDL_GetError() << '\n';
        exit(1);
    }
}