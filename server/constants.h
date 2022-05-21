#ifndef CONSTANTS_H
#define CONSTANT_H

#define PORT 2137
#define MAX_PLAYERS 4
#define MAX_PROJECTILES 64

#define UPDATE 'u'
#define CREATE 'c'
#define DISCONNECT 'd'
#define DIE 'i'

#define TANK 't'
#define PROJECTILE 'p'
#define TURRET 'r'

#define NOOWNER -1 //For projectiles - they need information who is owner. Tanks don't.
#define PROJECTILE_HP_ALIVE 1

#define DEFAULT_TANK_SKIN 0
#define TANK_SPAWN_POINT_X 400
#define TANK_SPAWN_POINT_Y 400
#define FULL_HP 100.00
#define NO_ROTATION 0.00

#define DEFAULT_MAP_NUMBER 0

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BACKGROUND_SCALE 50
#define USED_ID -1

//For function calculate_physics
#define OK 0
#define DISCONNECTED -1
#define DEAD -2

#define PROJECTILE_EXISTS 1
#define PROJECTILE_NOT_EXISTS 0
#define NEW_CLIENT_WAIT_TIME_SEC 5 //Set this to turn off the server using q

#define MAX_PROJECTILE_COUNT 20

#define RECEIVER_BUFFER_SIZE 28
#define CLIENT_MOVE_WAIT_SEC 0
#define CLIENT_MOVE_WAIT_USEC 20 //uSEC

#define COMMUNICATION_INTERVAL 30.0

//PROJECTILE ID'S
//PLAYER 0 -> 0--9
//PLAYER 1 -> 10--19
//...
#define MAX_PROJECTILES_PER_PLAYER 10


#endif