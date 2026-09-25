#pragma once

#include "raylib/raylib.h"

typedef enum EnemyState {
    ENEMY_STATE_PATROLLING,
    ENEMY_STATE_SUSPICIOUS,
    ENEMY_STATE_FOLLOWING,
} EnemyState;

typedef enum EnemyActionType {
    ENEMY_ACTION_TYPE_WALK,
    ENEMY_ACTION_TYPE_ROTATE,
    ENEMY_ACTION_TYPE_WAIT,
} EnemyActionType;

typedef union EnemyActionPayload {
    struct {
        Vector2 start;
        Vector2 end;
    } walk;
    struct {
        float start;
        float end;
    } rotate;
} EnemyActionPayload;

typedef struct EnemyAction {
    float totalTime;
    float currentTime;
    EnemyActionPayload payload;
    EnemyActionType type;
} EnemyAction;

typedef struct EnemyActions {
    int count;
    int current;
    EnemyAction actions[10];
} EnemyActions;

typedef struct Enemy {

    Vector2 pos;
    float radius;

    float outOfReachDistance;
    float warningDistance;
    float alertDistance;
    float fov;
    float angle;

    float followingSpeed;

    Color colors[3];
    EnemyState state;

    EnemyActions actions[3];

} Enemy;

void updateEnemy( Enemy *e, float delta );
void drawEnemy( Enemy *e );