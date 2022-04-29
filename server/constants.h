#ifndef CONSTANTS_H
#define CONSTANT_H

#define PORT 2137
#define MAX_PLAYERS 4
#define MAX_PROJECTILES 64

#define UPDATE 'u'
#define CREATE 'c'
#define DISCONNECT 'd'

#define TANK 't'
#define PROJECTILE 'p'
#define TURRET 'r'

#define NOOWNER -1 //For projectiles - they need information who is owner. Tanks don't.
#define PROJECTILE_HP_ALIVE 1

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BACKGROUND_SCALE 50
#define USED_ID -1

#define PROJECTILE_EXISTS 1
#define PROJECTILE_NOT_EXISTS 0

//Constants for receiver
#define RECEIVER_BUFFER_SIZE 512
#define CLIENT_MOVE_WAIT 5


#endif