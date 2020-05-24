#include "map.h"

void drawLineRaycaster(SDL_Renderer* gRenderer, struct Player* p)
{
    for (int x = 0; x < SCR_WIDTH; x++)
    {
        double cameraX = 2 * double(x) / double(SCR_WIDTH) - 1; // x coordinate in cameraspace
        double rayDirX = p->dirX + p->planeX * cameraX;
        double rayDirY = p->dirY + p->planeY * cameraX;

        int mapX = int(p->posX);
        int mapY = int(p->posY);

        double sideDistX, sideDistY, perpWallDist, deltaDistX, deltaDistY;

        deltaDistX = std::abs(1 / rayDirX);
        deltaDistY = std::abs(1 / rayDirY);

        int stepX, stepY, hit = 0, side;

        //calculate step and initial sideDist
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (p->posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - p->posX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (p->posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - p->posY) * deltaDistY;
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
        if (side == 0) perpWallDist = (mapX - p->posX + (1 - stepX) / 2) / rayDirX;
        else           perpWallDist = (mapY - p->posY + (1 - stepY) / 2) / rayDirY;

        //Calculate height of line to draw on screen
        int lineHeight = (int)(SCR_WIDTH / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        //int drawStart = int((-lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist) + p->pitch); // uncomment this for looking up and down MIGHT BrEAK THINGS
        int drawStart = int((-lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist));
        if (drawStart < 0)drawStart = 0;
        //int drawEnd = int((lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist) + p->pitch); // uncomment this for looking up and down MIGHT BrEAK THINGS
        int drawEnd = int((lineHeight / 2 + SCR_HEIGHT / 2) + (p->posZ / perpWallDist));
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
}