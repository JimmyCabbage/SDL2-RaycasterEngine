#include "player.h"
#include <iostream>

void readKeys(struct Player* p)
{

}

void playerMovement(struct Player* p, double frameTime)
{
    double oldDirX;
    double oldPlaneX;

    //speed modifiers
    p->rotSpeed = frameTime * 1.0; //the constant value is in radians/second
    float addedSpeed = 0.0f;

    if (p->jumping)
        addedSpeed = 0.1f; // boost the speed when jumping
    if (p->wsad[4])//sprinting
        addedSpeed = 0.11f;
    if (!p->wsad[4]&& !p->jumping)
        addedSpeed = 0.0f;
    if (p->wsad[4] && p->jumping)
        addedSpeed = 0.115f;

    //std::cout << addedSpeed << '\n';
    
    p->moveSpeed = frameTime * 3.0 + double(addedSpeed / 4); //the constant value is in squares/second


    if (p->wsad[5] && p->jumping == false && p->falling == false)
        p->jumping = true;
    if (p->jumping == true && !(p->posZ > 400))
    {
        p->posZ += 30; // moves up player by 5 pixels
    }
    if (p->jumping == true && p->posZ > 400)
    {
        p->jumping = false;
        p->falling = true;
    }
    if (p->falling == true && !(p->posZ <= 0))
    {
        p->posZ -= 35; // moves player down by 5 pixels
    }
    if (p->falling == true && p->posZ <= 0)
    {
        p->posZ = 0; // msets player to ground
        p->falling = false;
    }

    // don't use this || comment it out in release if you don't want looking up and down to save proccessing
    /*
    if (p->wsad[6])//look up
    {
        p->pitch += 5.0;
    }
    if (p->wsad[7])//look down
    {
        p->pitch -= 5.0;
    }
    */

    if (p->wsad[0])//move forward
    {
        if (worldMap[int(p->posX + p->dirX * p->moveSpeed)][int(p->posY)] == false) p->posX += p->dirX * p->moveSpeed;
        if (worldMap[int(p->posX)][int(p->posY + p->dirY * p->moveSpeed)] == false) p->posY += p->dirY * p->moveSpeed;
    }
    if (p->wsad[1])//move backward
    {
        if (worldMap[int(p->posX - p->dirX * p->moveSpeed)][int(p->posY)] == false) p->posX -= p->dirX * p->moveSpeed;
        if (worldMap[int(p->posX)][int(p->posY - p->dirY * p->moveSpeed)] == false) p->posY -= p->dirY * p->moveSpeed;
    }
    if (p->wsad[2])//turn left
    {
        //both camera direction and camera plane must be rotated
        oldDirX = p->dirX;
        p->dirX = p->dirX * cos(-(p->rotSpeed)) - p->dirY * sin(-(p->rotSpeed));
        p->dirY = oldDirX * sin(-(p->rotSpeed)) + p->dirY * cos(-(p->rotSpeed));
        oldPlaneX = p->planeX;
        p->planeX = p->planeX * cos(-(p->rotSpeed)) - p->planeY * sin(-(p->rotSpeed));
        p->planeY = oldPlaneX * sin(-(p->rotSpeed)) + p->planeY * cos(-(p->rotSpeed));
    }
    if (p->wsad[3])//turn right
    {
        //both camera direction and camera plane must be rotated
        oldDirX = p->dirX;
        p->dirX = p->dirX * cos(p->rotSpeed) - p->dirY * sin(p->rotSpeed);
        p->dirY = oldDirX * sin(p->rotSpeed) + p->dirY * cos(p->rotSpeed);
        oldPlaneX = p->planeX;
        p->planeX = p->planeX * cos(-(p->rotSpeed)) - p->planeY * sin(p->rotSpeed);
        p->planeY = oldPlaneX * sin(p->rotSpeed) + p->planeY * cos(p->rotSpeed);
    }
}