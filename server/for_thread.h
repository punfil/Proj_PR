#ifndef FOR_THREAD_H
#define FOR_THREAD_H
#include "projectile.h"
#include "tank.h"

struct for_thread;

struct for_thread* for_thread_alloc();

void for_thread_set_values(struct for_thread* self, int* player_ids, struct tank** tanks_in_game, struct projectile** projectiles_in_game, int** csockets, struct sockaddr_in** clients);

void for_thread_free(struct for_thread* self);

#endif