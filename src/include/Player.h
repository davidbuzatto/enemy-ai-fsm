#pragma once

#include "raylib/raylib.h"

typedef struct Player {
    Vector2 pos;
    float radius;
    float walkingSpeed;
    Color color;
} Player;

void updatePlayer( Player *p, float delta );
void drawPlayer( Player *p );
