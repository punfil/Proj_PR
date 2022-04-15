#ifndef FOR_THREAD_H
#define FOR_THREAD_H
#ifndef PROJECTILE_H
#include "projectile.h"
#endif

#ifndef TANK_H
#include "tank.h"
#endif

struct for_thread {
	int* player_ids;
    struct tank** tanks_in_game;
    struct projectile** projectiles_in_game;
    int** csockets;
    struct sockaddr_in** clients;
};

struct for_thread* for_thread_alloc();

void for_thread_set_values(struct for_thread* self, int* player_ids, struct tank** tanks_in_game, struct projectile** projectiles_in_game, int** csockets, struct sockaddr_in** clients);

void for_thread_free(struct for_thread* self);

#endif