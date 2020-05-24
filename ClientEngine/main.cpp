#include <cstdlib>
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

const int SCR_HEIGHT = 720;
const int SCR_WIDTH = 1280;

const int mapWidth = 24;
const int mapHeight = 24;

struct ColorRGB
{
    int r, g, b;
};

int worldMap[mapWidth][mapHeight] =
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
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


int main(int argc, char** argv)
{
    bool sprinting = false;
    bool jumping = false;
    bool falling = false;

    int wsad[6] = { 0, 0, 0, 0, 0, 0 };

    double moveSpeed, rotSpeed;

    double frameTime;

    double posX = 22, posY = 12, posZ = 0; // x, y, and z position in start
    double dirX = -1, dirY = 0; // direction when start
    double planeX = 0, planeY = 0.66; // 2d version of camera plane

    double time = 0; //time of current frame
    double oldTime = 0; // time of the previous frame

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "Could not initiate SDL: " << SDL_GetError() << '\n';
		exit(1);
	}

	SDL_Window* gWindow = SDL_CreateWindow("Raycaster in SDL2 Port", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT, SDL_WINDOW_SHOWN);

	if (!gWindow)
	{
		std::cerr << "Unable to create SDL window: " << SDL_GetError() << '\n';
		exit(1);
	}

	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);

	if (!gRenderer)
	{
		std::cerr << "Unable to create SDL renderer: " << SDL_GetError() << '\n';
		exit(1);
	}
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    while (1)
    {
        for (int x = 0; x < SCR_WIDTH; x++)
        {
            double cameraX = 2 * double(x) / double(SCR_WIDTH) - 1; // x coordinate in cameraspace
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = int(posX);
            int mapY = int(posY);

            double sideDistX, sideDistY, perpWallDist, deltaDistX, deltaDistY;

            deltaDistX = std::abs(1 / rayDirX);
            deltaDistY = std::abs(1 / rayDirY);

            int stepX, stepY, hit = 0, side;

            //calculate step and initial sideDist
            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
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
            if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
            else           perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

            //Calculate height of line to draw on screen
            int lineHeight = (int)(SCR_WIDTH / perpWallDist);

            //calculate lowest and highest pixel to fill in current stripe
            int drawStart = -lineHeight / 2 + SCR_HEIGHT / 2;
            if (drawStart < 0)drawStart = 0;
            int drawEnd = lineHeight / 2 + SCR_HEIGHT / 2;
            if (drawEnd >= SCR_HEIGHT)drawEnd = SCR_HEIGHT - 1;

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
            default://yellow
                color.r = 255;
                color.g = 255;
                color.b = 0;
                break;
            }

            //give x and y sides different brightness
            if (side == 1)
            {
                color.r = color.r / 2;
                color.g = color.g / 2;
                color.b = color.b / 2;
            }

            //draw the pixels of the stripe as a vertical linefor the ceiling
            SDL_SetRenderDrawColor(gRenderer, 200, 200, 200, 255);
            SDL_RenderDrawLine(gRenderer, x, 0, x, drawStart);

            //draw the pixels of the stripe as a vertical linefor floor
            SDL_SetRenderDrawColor(gRenderer, 100, 100, 100, 255);
            SDL_RenderDrawLine(gRenderer, x, drawEnd, x, SCR_HEIGHT);

            //draw the pixels of the stripe as a vertical line for the walls
            SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, 255);
            SDL_RenderDrawLine(gRenderer, x, drawStart, x, drawEnd);
        }

        SDL_RenderPresent(gRenderer);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        //timing for input and FPS counter
        oldTime = time;
        time = SDL_GetTicks();
        frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
        std::cout<< (1.0 / frameTime) << '\n'; //FPS counter

        SDL_Event event;
        double oldDirX;
        double oldPlaneX;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                goto done;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case 'w':
                    wsad[0] = event.type == SDL_KEYDOWN;
                    break;
                case 's':
                    wsad[1] = event.type == SDL_KEYDOWN;
                    break;
                case 'a': 
                    wsad[3] = event.type == SDL_KEYDOWN;
                    break;
                case 'd':
                    wsad[2] = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_LSHIFT:
                    wsad[4] = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_SPACE:
                    wsad[5] = event.type == SDL_KEYDOWN;
                    break;
                case SDLK_ESCAPE:
                    goto done;
                case 'q': goto done;
                default: break;
                }

            default:
                break;
            }
        }
        //speed modifiers
        moveSpeed = frameTime * 3.0; //the constant value is in squares/second
        rotSpeed = frameTime * 1.0; //the constant value is in radians/second
        if (wsad[4])
        {
            //speed modifiers
            moveSpeed = frameTime * 6.5; //the constant value is in squares/second
            rotSpeed = frameTime * 1.0; //the constant value is in radians/second
        }
        if (wsad[5] && jumping == false)
            jumping = true;
        if (jumping == true)
        {

        }

        if (wsad[0])
        {
            if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }
        if (wsad[1])
        {
            if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
        if (wsad[2])
        {
            //both camera direction and camera plane must be rotated
            oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        if (wsad[3])
        {
            //both camera direction and camera plane must be rotated
            oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(-rotSpeed);
            oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
    }

done:
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    SDL_Quit();
    return 0;
}