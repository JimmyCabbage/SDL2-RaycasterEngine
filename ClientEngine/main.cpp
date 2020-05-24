#include <cstdlib>
#include <iostream>
#include <SDL2/SDL.h>

#include "variables.h"
#include "maths.h"
#include "setup.h"

/*
/ Unpublished work Copywrite © 2020 Ryan Rhee
/ All rights reserved
/
/
*/


int main(int argc, char** argv)
{
    setupSDL2();
    
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    std::cout << "Unpublished work Copyright 2020 Ryan Rhee\nAll Rights Reserved\n";

    while (1)
    {
        for (int x = 0; x < SCR_WIDTH; x++)
        {
            maths(x);

            //choose wall color
            ColorRGBA color;
            color.a = 255;
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
            case 9://cyan
                color.r = 0;
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
            SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
            SDL_RenderDrawLine(gRenderer, x, drawStart, x, drawEnd);
        }

        SDL_RenderPresent(gRenderer);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);

        //timing for input and FPS counter
        oldTime = time;
        time = SDL_GetTicks();
        frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
        //std::cout<< (1.0 / frameTime) << '\n'; //prints FPS

        SDL_Event event;

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
        rotSpeed = frameTime * 1.0; //the constant value is in radians/second

        if (jumping)
            moveSpeed = frameTime * 5.0 + (rotSpeed * 2); // boost the speed when jumping
        else if (wsad[4])//sprinting
            moveSpeed = frameTime * 6.0;
        else
            moveSpeed = frameTime * 3.0; //the constant value is in squares/second


        if (wsad[5] && jumping == false && falling == false)
            jumping = true;
        if (jumping == true && !(posZ > 400))
        {
            posZ += 50; // moves up player by 5 pixels
        }
        if (jumping == true && posZ > 400)
        {
            jumping = false;
            falling = true;
        }
        if (falling == true && !(posZ <= 0))
        {
            posZ -= 5; // moves player down by 5 pixels
        }
        if (falling == true && posZ <= 0)
        {
            posZ = 0; // msets player to ground
            falling = false;
        }
        
        if (wsad[0])//move forward
        {
            if (worldMap[int(posX + dirX * (moveSpeed + rotSpeed / 2))][int(posY)] == false) posX += dirX * (moveSpeed + rotSpeed / 2);
            if (worldMap[int(posX)][int(posY + dirY * (moveSpeed + rotSpeed / 2))] == false) posY += dirY * (moveSpeed + rotSpeed / 2);
        }
        if (wsad[1])//move backward
        {
            if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
        if (wsad[2])//turn left
        {
            //both camera direction and camera plane must be rotated
            oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        if (wsad[3])//turn right
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

    std::cin.ignore();
    return 0;
}