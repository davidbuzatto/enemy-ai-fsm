#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "Enemy.h"
#include "Macros.h"
#include "Player.h"

static void drawEnemyFSM( Enemy *e );

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

static const Color colorTable[] = {
    [ENEMY_STATE_PATROLLING] = { 0, 200, 40, 255 },
    [ENEMY_STATE_SUSPICIOUS] = { 215, 215, 0, 255 },
    [ENEMY_STATE_FOLLOWING] = { 255, 41, 55, 255 },
};

void updateEnemy( Enemy *e, Player *p, float delta ) {

    EnemyActions *actions = &e->actions[e->state];

    if ( e->state == ENEMY_STATE_PATROLLING ) {

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

    }

    Vector2 forward = {
        cosf( DEG2RAD * e->angle ),
        sinf( DEG2RAD * e->angle ),
    };

    Vector2 offset = Vector2Subtract( p->pos, e->pos );
    float distance = Vector2Distance( p->pos, e->pos );
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

    DrawCircleV( e->pos, e->outOfReachDistance, Fade( GRAY, 0.05f ) );

    DrawCircleSector( 
        e->pos, e->warningDistance, 
        e->angle - e->fov, e->angle + e->fov, 
        10, WHITE
    );

    DrawCircleSector( 
        e->pos, e->warningDistance, 
        e->angle - e->fov, e->angle + e->fov, 
        10, Fade( colorTable[ENEMY_STATE_SUSPICIOUS], 0.7f )
    );

    DrawCircleSector( 
        e->pos, e->alertDistance, 
        e->angle - e->fov, e->angle + e->fov, 
        10, Fade( colorTable[ENEMY_STATE_FOLLOWING], 0.7f )
    );

    DrawCircleV( e->pos, e->radius, colorTable[e->state] );
    DrawCircleLinesV( e->pos, e->radius, BLACK );
    drawEnemyFSM( e );

}

static void drawEnemyFSM( Enemy *e ) {

    EnemyActions *actions = &e->actions[e->state];
    DrawText( enemyStateTable[e->state], e->pos.x + 2, e->pos.y - e->radius - 40 + 2, 20, BLACK );
    DrawText( enemyStateTable[e->state], e->pos.x, e->pos.y - e->radius - 40, 20, colorTable[e->state] );

    if ( e->state == ENEMY_STATE_PATROLLING ) {
        EnemyAction *action = &actions->actions[actions->current % actions->count];
        DrawText( enemyActionTypeTable[action->type], e->pos.x + 2, e->pos.y - e->radius - 20 + 2, 20, BLACK );
        DrawText( enemyActionTypeTable[action->type], e->pos.x, e->pos.y - e->radius - 20, 20, colorTable[e->state] );
    }

}