#include "maths.h"

void maths(int x)
{
    cameraX = 2 * double(x) / double(SCR_WIDTH) - 1; // x coordinate in cameraspace
    rayDirX = dirX + planeX * cameraX;
    rayDirY = dirY + planeY * cameraX;

    mapX = int(posX);
    mapY = int(posY);

    deltaDistX = std::abs(1 / rayDirX);
    deltaDistY = std::abs(1 / rayDirY);

    hit = 0;

    //calculate step and initial sideDist AKA math
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

    while (hit == 0)//more math to check walls
    {
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

    //Corrects perspective
    if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
    else           perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

    //Calculate height of line to draw on screen
    int lineHeight = (int)(SCR_WIDTH / perpWallDist);

    //calculate lowest and highest pixel to fill in current stripe
    drawStart = int((-lineHeight / 2 + SCR_HEIGHT / 2) + (posZ / perpWallDist) + pitch);
    if (drawStart < 0)drawStart = 0;
    drawEnd = int((lineHeight / 2 + SCR_HEIGHT / 2) + (posZ / perpWallDist) + pitch);
    if (drawEnd >= SCR_HEIGHT)drawEnd = SCR_HEIGHT - 1;
}