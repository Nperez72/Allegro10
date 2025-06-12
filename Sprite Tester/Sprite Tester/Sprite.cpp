#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>
#include <stdio.h>
#include "Sprite.h"
#include <iostream>
#include <ctime>
using namespace std;

sprite::sprite()
{
    // Randomly assign type (SCARED, BABY, SPINNING, or FREEZE)
    int t = rand() % 4;
    if (t == 0) type = SCARED;
    else if (t == 1) type = BABY;
    else if (t == 2) type = SPINNING;
    else type = FREEZE;
    // Initialize each property for each type
    scaredColor = al_map_rgb(255, 255, 255);

    babyScale = 1.0f;
    babyLastScaleTime = 0;

    spinAngle = 0.0f;
    spinSpeed = 0.1f + 0.1f * (rand() % 5);

    isFrozen = false;
    freezeStartTime = 0;
    x = y = 0;
}

sprite::~sprite()
{
    for (int i = 0; i < maxframe; i++)
        al_destroy_bitmap(image[i]);
}

void sprite::load_animated_sprite(int size)
{
    char s[80];
    maxframe = size;
    for (int n = 0; n < size; n++)
    {
        sprintf_s(s, "Alien%d.bmp", n);
        image[n] = al_load_bitmap(s);
        al_convert_mask_to_alpha(image[n], al_map_rgb(255, 255, 255));
    }
    width = al_get_bitmap_width(image[0]);
    height = al_get_bitmap_height(image[0]);
    curframe = 0;
    framedelay = 5;
    framecount = 0;
}

void sprite::drawSprite()
{
    if (type == SCARED)
    {
        al_draw_tinted_bitmap(image[curframe], scaredColor, x, y, 0);
    }
    else if (type == BABY)
    {
        al_draw_scaled_bitmap(
            image[curframe],
            0, 0, width, height,
            x, y,
            width * babyScale, height * babyScale,
            0);
    }
    else if (type == SPINNING)
    {
        // Draw rotated around the center
        float cx = width / 2.0f;
        float cy = height / 2.0f;
        al_draw_rotated_bitmap(
            image[curframe],
            cx, cy,
            x + cx, y + cy, // Position to draw (centered)
            spinAngle, 0
        );
    }
    else
    {
        al_draw_bitmap(image[curframe], x, y, 0);
    }
}

void sprite::updatesprite()
{
    // FREEZE: skip all updates if frozen
    if (type == FREEZE && isFrozen) {
        double now = al_get_time();
        if (now - freezeStartTime >= 5.0) {
            isFrozen = false; // Unfreeze after 5 seconds
        } else {
            return; // Skip movement and animation
        }
    }

    // update x position
    if (++xcount > xdelay)
    {
        xcount = 0;
        x += xspeed;
    }

    // update y position
    if (++ycount > ydelay)
    {
        ycount = 0;
        y += yspeed;
    }

    if (framecount++ > framedelay)
    {
        framecount = 0;
        curframe++;
        if (curframe >= maxframe)
            curframe = 0;
    }

    // BABY: restore scale after 10 seconds
    if (type == BABY && babyScale < 1.0f)
    {
        double now = al_get_time();
        if (babyLastScaleTime > 0 && now - babyLastScaleTime >= 10.0)
        {
            babyScale = 1.0f;
            babyLastScaleTime = 0;
        }
    }

    // SPINNING: update angle
    if (type == SPINNING)
    {
        spinAngle += spinSpeed;
        if (spinAngle > ALLEGRO_PI * 2)
            spinAngle -= ALLEGRO_PI * 2;
    }
}

void sprite::bouncesprite(int SCREEN_W, int SCREEN_H)
{
    if (x < 0)
    {
        x = 0;
        xspeed = rand() % 2 + 4;
        animdir *= -1;
    }
    else if (x > SCREEN_W - width)
    {
        x = SCREEN_W - width;
        xspeed = rand() % 2 - 6;
        animdir *= -1;
    }

    if (y < 0)
    {
        y = 0;
        yspeed = rand() % 2 + 4;
        animdir *= -1;
    }
    else if (y > SCREEN_H - height)
    {
        y = SCREEN_H - height;
        yspeed = rand() % 2 - 6;
        animdir *= -1;
    }
}

bool sprite::collidesWith(sprite& other)
{
    // Calculate scaled width/height for BABY type
    int w1 = (type == BABY) ? int(width * babyScale) : width;
    int h1 = (type == BABY) ? int(height * babyScale) : height;
    int w2 = (other.type == BABY) ? int(other.width * other.babyScale) : other.width;
    int h2 = (other.type == BABY) ? int(other.height * other.babyScale) : other.height;

    return !(x + w1 < other.x || x > other.x + w2 ||
             y + h1 < other.y || y > other.y + h2);
}

void sprite::onCollision(sprite* other)
{
    if (type == SCARED)
    {
        // Change to a random color and teleport
        scaredColor = al_map_rgb(rand() % 256, rand() % 256, rand() % 256);
        x = rand() % 500;
        y = rand() % 400;
    }
    else if (type == BABY)
    {
        // Scale in half, teleport, and set timer
        if (babyScale > 0.01f)
        {
            babyScale *= 0.5f;
            x = rand() % 500;
            y = rand() % 400;
            babyLastScaleTime = al_get_time();
            if (babyScale < 0.01f)
            {
                std::cout << "The baby sprite died!" << std::endl;
            }
        }
    }
    // Freeze movement if true
    else if (type == FREEZE)
    {
        isFrozen = true;
        freezeStartTime = al_get_time();
    }
}



