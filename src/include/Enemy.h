#pragma once

#include "raylib/raylib.h"

typedef enum EnemyState {
    ENEMY_STATE_PATROLLING,
    ENEMY_STATE_SUSPICIOUS,
    ENEMY_STATE_FOLLOWING,
} EnemyState;

typedef struct Enemy {

    Vector2 pos;
    float radius;

    float followingSpeed;

    Color colors[3];
    EnemyState state;

} Enemy;

void updateEnemy( Enemy *e, float delta );
void drawEnemy( Enemy *e );