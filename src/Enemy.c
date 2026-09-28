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
    [ENEMY_STATE_CHASING] = "Chasing",
};

static const char *enemyActionTypeTable[] = {
    [ENEMY_ACTION_TYPE_WALK] = "Walking",
    [ENEMY_ACTION_TYPE_ROTATE] = "Rotating",
    [ENEMY_ACTION_TYPE_WAIT] = "Waiting",
};

static const Color colorTable[] = {
    [ENEMY_STATE_PATROLLING] = { 0, 200, 40, 255 },
    [ENEMY_STATE_SUSPICIOUS] = { 215, 215, 0, 255 },
    [ENEMY_STATE_CHASING] = { 255, 41, 55, 255 },
};


static void prepareSuspiciousActions( Enemy *e );
static void resetSuspiciousData( Enemy *e );
static void resetChasingData( Enemy *e );

void updateEnemy( Enemy *e, Player *p, float delta ) {

    EnemyActions *actions = &e->actions[e->state];

    if ( e->state == ENEMY_STATE_PATROLLING || e->state == ENEMY_STATE_SUSPICIOUS ) {

        if ( e->state == ENEMY_STATE_SUSPICIOUS ) {
            trace( "%d %d", actions->current, actions->count );
        }
        
        if ( actions->count > 0 ) {

            EnemyAction *action = &actions->steps[actions->current % actions->count];

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

    } else if ( e->state == ENEMY_STATE_CHASING ) {

    }

    if ( e->state != ENEMY_STATE_CHASING ) {

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
                e->state = ENEMY_STATE_CHASING;
                if ( !e->chasingData.ready ) {
                    e->chasingData.ready = true;
                    trace( "chasing..." );
                }
            } else {
                e->state = ENEMY_STATE_PATROLLING;
                resetSuspiciousData( e );
                resetChasingData( e );
            }
        } else if ( distance > 0 && distance < e->warningDistance ) {
            if ( dot > cosFov ) {
                e->state = ENEMY_STATE_SUSPICIOUS;
                if ( !e->suspiciousData.ready ) {
                    e->suspiciousData.ready = true;
                    e->suspiciousData.playerPos = p->pos;
                    prepareSuspiciousActions( e );
                }
            } else {
                e->state = ENEMY_STATE_PATROLLING;
                resetSuspiciousData( e );
                resetChasingData( e );
            }
        } else {
            e->state = ENEMY_STATE_PATROLLING;
            resetSuspiciousData( e );
            resetChasingData( e );
        }

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
        10, Fade( colorTable[ENEMY_STATE_CHASING], 0.7f )
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
        EnemyAction *action = &actions->steps[actions->current % actions->count];
        DrawText( enemyActionTypeTable[action->type], e->pos.x + 2, e->pos.y - e->radius - 20 + 2, 20, BLACK );
        DrawText( enemyActionTypeTable[action->type], e->pos.x, e->pos.y - e->radius - 20, 20, colorTable[e->state] );
    }

}

static void prepareSuspiciousActions( Enemy *e ) {

    trace( "preparing suspicious actions..." );

    EnemyActions *actions = &e->actions[ENEMY_STATE_SUSPICIOUS];

    actions->count = 0;
    actions->current = 0;

    float endAngle = RAD2DEG * atan2f( 
        e->suspiciousData.playerPos.y - e->pos.y,
        e->suspiciousData.playerPos.x - e->pos.x
    );

    actions->steps[actions->count++] = (EnemyAction) {
        .totalTime = 0.2f,
        .currentTime = 0,
        .payload = {
            .rotate = {
                .start = e->angle,
                .end = endAngle - e->angle
            }
        },
        .type = ENEMY_ACTION_TYPE_ROTATE
    };

    actions->steps[actions->count++] = (EnemyAction) {
        .totalTime = 2,
        .currentTime = 0,
        .payload = {
            .walk = {
                .start = e->pos,
                .end = e->suspiciousData.playerPos
            }
        },
        .type = ENEMY_ACTION_TYPE_WALK
    };

}

static void resetSuspiciousData( Enemy *e ) {
    e->suspiciousData.ready = false;
    e->actions[ENEMY_STATE_SUSPICIOUS].count = 0;
}

static void resetChasingData( Enemy *e ) {
    e->chasingData.ready = false;
    e->actions[ENEMY_STATE_CHASING].count = 0;
}