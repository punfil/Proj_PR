#ifndef FOR_THREAD_H
#define FOR_THREAD_H
#ifndef PROJECTILE_H
#include "projectile.h"
#endif

#ifndef TANK_H
#include "tank.h"
#endif

#ifndef SINGLY_LINKED_LIST_H
#include "singly_linked_list.h"
#endif

struct for_thread {
	int* player_ids;
    struct tank** tanks_in_game;
    struct singly_linked_node* projectiles_in_game;
    int** csockets;
    struct sockaddr_in** clients;
    bool* running;
    int* players_count;
};

struct for_thread* for_thread_alloc();

void for_thread_set_values(struct for_thread* self, int* player_ids, struct tank** tanks_in_game, struct singly_linked_node* projectiles_in_game, int** csockets, struct sockaddr_in** clients, int* players_count);

void for_thread_free(struct for_thread* self);

#endif