#ifndef CONSTANTS_H
#define CONSTANT_H

//Game settings
#define MAX_PLAYERS 7
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BACKGROUND_SCALE 50
#define USED_ID -1

//Communication settings
#define PORT 2137
#define SERVER_EXIT_BUTTON 'q'
#define FULL_SERVER_INFO -99
#define NEW_CLIENT_WAIT_TIME_SEC 5 //Set this to turn off the server using SERVER_EXIT_BUTTON
#define RECEIVER_BUFFER_SIZE sizeof(struct information) //largest struct
#define CLIENT_MOVE_WAIT_SEC 0
#define CLIENT_MOVE_WAIT_USEC 20
#define CLIENT_NO_RESPONSE_ITERATION 100000
#define INFORMATION_NOT_REQUIRED 0

//For information.action
#define UPDATE 'u'
#define CREATE 'c'
#define DISCONNECT 'd'
#define DIE 'i'

//For information.type_of
#define TANK 't'
#define PROJECTILE 'p'

//Projectiles settings
#define PROJECTILE_EXISTS 1
#define PROJECTILE_NOT_EXISTS 0
#define POSITION_NOT_REQUIRED 0
//PROJECTILE ID'S
//PLAYER 0 -> 0--9
//PLAYER 1 -> 10--19
//...
//PLAYER N -> MAX_PROJECTILES_PER_PLAYER*N -- MAX_PROJECTILES_PER_PLAYER(N+1)-1 //Calculated on the client side!
#define MAX_PROJECTILES_PER_PLAYER 20

//Tank settings
#define DEFAULT_MAP_NUMBER 0
#define DEFAULT_TANK_VERSION 0
#define TANK_SPAWN_POINT_X -400.0
#define TANK_SPAWN_POINT_Y -400.0
#define FULL_HP 10.0
#define EMPTY_HP 0.0
#define NO_ROTATION 0.0
#define DEFAULT_TANK_ANGLE 0.0
#define DEFAULT_TANK_TURRET_ANGLE 0.0
#define TANK_COLLISION_R 25 //In pixels - Treat tank as circle when dealing with collisions
#define TANK_PROJECTILE_COLLISION_DAMAGE 2.5
#define SHIELD_ACTIVE true
#define SHIELD_INACTIVE false

//States of players for function calculate_physics
#define OK 0
#define DISCONNECTED -1
#define DEAD -2
#define CONNECTION_LOST -3

#endif