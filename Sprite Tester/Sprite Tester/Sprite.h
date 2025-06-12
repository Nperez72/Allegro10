#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>

enum SpriteType {
    NORMAL,
    SCARED,
    BABY
};

class sprite
{
public: 
    sprite(); // Constructor for random assignment
    ~sprite();
    void updatesprite();
    void bouncesprite(int SCREEN_W, int SCREEN_H);
    void load_animated_sprite(int size);
    void drawSprite();
    int getX(){return x;}
    int getY(){return y;}
    int getWidth() { return width; }
    int getHeight() { return height; }
    void onCollision(sprite* other); // Checks for collision between sprites

    SpriteType getType() { return type; }

    // Helper method for collision check
    bool collidesWith(sprite& other);

private:
    // Common sprite data
    int x, y;
    int width, height;
    int xspeed, yspeed;
    int xdelay, ydelay;
    int xcount, ycount;
    int curframe, maxframe, animdir;
    int framecount, framedelay;
    ALLEGRO_BITMAP *image[9];

    // Specialty skill type
    SpriteType type = NORMAL;

    // For SCARED
    ALLEGRO_COLOR scaredColor = al_map_rgb(255,255,255);

    // For BABY
    float babyScale = 1.0f;
    double babyLastScaleTime = 0;
};