#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "Enemy.h"
#include "Macros.h"

static void drawEnemyFSM( Enemy *e );

static Vector2 testPos = { 20, 20 };

static const char *enemyStateTable[] = {
    [ENEMY_STATE_PATROLLING] = "Patrolling",
    [ENEMY_STATE_SUSPICIOUS] = "Suspicious",
    [ENEMY_STATE_FOLLOWING] = "Following",
};

static const char *enemyActionTypeTable[] = {
    [ENEMY_ACTION_TYPE_WALK] = "Walking",
    [ENEMY_ACTION_TYPE_ROTATE] = "Rotating",
    [ENEMY_ACTION_TYPE_WAIT] = "Waiting",
};

void updateEnemy( Enemy *e, float delta ) {

    if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        testPos = GetMousePosition();
    }

    /*if ( IsKeyDown( KEY_RIGHT ) ) {
        e->angle++;
    }

    if ( IsKeyDown( KEY_LEFT ) ) {
        e->angle--;
    }*/

    //if ( e->state == ENEMY_STATE_PATROLLING ) {

        //EnemyActions *actions = &e->actions[e->state];
        EnemyActions *actions = &e->actions[ENEMY_STATE_PATROLLING];
        EnemyAction *action = &actions->actions[actions->current % actions->count];

        switch ( action->type ) {

            case ENEMY_ACTION_TYPE_WALK: {
                float perc = action->currentTime / action->totalTime;
                e->pos = Vector2Lerp( action->payload.walk.start, action->payload.walk.end, perc );
                break;
            }

            case ENEMY_ACTION_TYPE_ROTATE: {
                float perc = action->currentTime / action->totalTime;
                e->angle = Lerp( action->payload.rotate.start, action->payload.rotate.end, perc );
                break;
            }

            case ENEMY_ACTION_TYPE_WAIT:
                break;

            default:
                break;

        }

        action->currentTime += delta;
        if ( action->currentTime > action->totalTime ) {
            action->currentTime = 0;
            actions->current++;
        }

    //}

    Vector2 forward = {
        cosf( DEG2RAD * e->angle ),
        sinf( DEG2RAD * e->angle ),
    };

    Vector2 offset = Vector2Subtract( testPos, e->pos );
    float distance = Vector2Distance( testPos, e->pos );
    Vector2 direction = { offset.x / distance, offset.y / distance };
    float dot = Vector2DotProduct( forward, direction );
    float cosFov = cosf( DEG2RAD * e->fov );

    if ( distance > 0 && distance <= e->alertDistance ) {
        if ( dot > cosFov ) {
            e->state = ENEMY_STATE_FOLLOWING;
        } else {
            e->state = ENEMY_STATE_PATROLLING;
        }
    } else if ( distance > 0 && distance < e->warningDistance ) {
        if ( dot > cosFov ) {
            e->state = ENEMY_STATE_SUSPICIOUS;
        } else {
            e->state = ENEMY_STATE_PATROLLING;
        }
    } else {
        e->state = ENEMY_STATE_PATROLLING;
    }

}

void drawEnemy( Enemy *e ) {

    DrawCircleV( testPos, 10, GREEN );

    DrawCircleSector( 
        e->pos, 
        e->outOfReachDistance, 
        e->angle - e->fov, 
        e->angle + e->fov, 
        10, 
        Fade( GRAY, 0.3f )
    );

    DrawCircleSector( 
        e->pos, 
        e->warningDistance, 
        e->angle - e->fov, 
        e->angle + e->fov, 
        10, 
        Fade( YELLOW, 0.3f )
    );

    DrawCircleSector( 
        e->pos, 
        e->alertDistance, 
        e->angle - e->fov, 
        e->angle + e->fov, 
        10, 
        Fade( RED, 0.3f )
    );

    DrawCircleV( e->pos, e->radius, e->colors[e->state] );
    drawEnemyFSM( e );

}

static void drawEnemyFSM( Enemy *e ) {

    EnemyActions *actions = &e->actions[e->state];
    DrawText( enemyStateTable[e->state], e->pos.x, e->pos.y - e->radius - 40, 20, e->colors[e->state] );

    if ( e->state == ENEMY_STATE_PATROLLING ) {
        EnemyAction *action = &actions->actions[actions->current % actions->count];
        DrawText( enemyActionTypeTable[action->type], e->pos.x, e->pos.y - e->radius - 20, 20, e->colors[e->state] );
    }

}