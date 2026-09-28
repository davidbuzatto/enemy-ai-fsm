/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "GameWorld.h"
#include "ResourceManager.h"

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld *createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    Vector2 enemyPosStart = { GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 };
    Vector2 enemyPosEnd = { GetScreenWidth() / 2 + 200, GetScreenHeight() / 2 };
    float enemyAngle = 0;

    float walkTime = 4;
    float idleTime = 0.8f;
    float rotationTime = 1;

    gw->player = (Player) {
        .pos = { 20, 20 },
        .radius = 15,
        .walkingSpeed = 200,
        .color = BLUE
    };

    gw->enemy = (Enemy) {
        .pos = enemyPosStart,
        .radius = 15,
        .outOfReachDistance = 180,
        .warningDistance = 150,
        .alertDistance = 80,
        .fov = 35,
        .angle = enemyAngle,
        .followingSpeed = 100,
        .state = ENEMY_STATE_PATROLLING,
        .actions = {
            [ENEMY_STATE_PATROLLING] = {
                .count = 6,
                .current = 0,
                .actions = {
                    {
                        .totalTime = walkTime,
                        .currentTime = 0,
                        .payload = {
                            .walk = {
                                .start = enemyPosStart,
                                .end = enemyPosEnd
                            }
                        },
                        .type = ENEMY_ACTION_TYPE_WALK
                    }, {
                        .totalTime = idleTime,
                        .currentTime = 0,
                        .payload = { 0 },
                        .type = ENEMY_ACTION_TYPE_WAIT
                    }, {
                        .totalTime = rotationTime,
                        .currentTime = 0,
                        .payload = {
                            .rotate = {
                                .start = enemyAngle,
                                .end = enemyAngle + 180
                            }
                        },
                        .type = ENEMY_ACTION_TYPE_ROTATE
                    }, {
                        .totalTime = walkTime,
                        .currentTime = 0,
                        .payload = {
                            .walk = {
                                .start = enemyPosEnd,
                                .end = enemyPosStart
                            }
                        },
                        .type = ENEMY_ACTION_TYPE_WALK
                    }, {
                        .totalTime = idleTime,
                        .currentTime = 0,
                        .payload = { 0 },
                        .type = ENEMY_ACTION_TYPE_WAIT
                    }, {
                        .totalTime = rotationTime,
                        .currentTime = 0,
                        .payload = {
                            .rotate = {
                                .start = enemyAngle + 180,
                                .end = enemyAngle + 360
                            }
                        },
                        .type = ENEMY_ACTION_TYPE_ROTATE
                    },
                }
            },
            [ENEMY_STATE_SUSPICIOUS] = { 0 },
            [ENEMY_STATE_CHASING] = { 0 },
        },
    };

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {
    updatePlayer( &gw->player, delta );
    updateEnemy( &gw->enemy, &gw->player, delta );
}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( BLACK );

    drawEnemy( &gw->enemy );
    drawPlayer( &gw->player );

    EndDrawing();

}