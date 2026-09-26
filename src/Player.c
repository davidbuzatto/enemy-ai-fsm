#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Player.h"

void updatePlayer( Player *p, float delta ) {

    int h = 0;
    int v = 0;

    h += IsKeyDown( KEY_LEFT ) ? -1 : 0;
    h += IsKeyDown( KEY_RIGHT ) ? 1 : 0;
    v += IsKeyDown( KEY_UP ) ? -1 : 0;
    v += IsKeyDown( KEY_DOWN ) ? 1 : 0;

    float walkingSpeed = p->walkingSpeed;
    if ( h != 0 && v != 0 ) {
        walkingSpeed = p->walkingSpeed * sinf( RAD2DEG * 45 );
    }
    
    p->pos.x += walkingSpeed * h * delta;
    p->pos.y += walkingSpeed * v * delta;

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        p->pos = GetMousePosition();
    }

}

void drawPlayer( Player *p ) {
    DrawCircleV( p->pos, p->radius, p->color );
}