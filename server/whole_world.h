#ifndef WHOLE_WORLD_H
#define WHOLE_WORLD_H
#ifndef PROJECTILE_H
#include "projectile.h"
#endif

#ifndef TANK_H
#include "tank.h"
#endif

#ifndef SINGLY_LINKED_LIST_H
#include "singly_linked_list.h"
#endif

struct whole_world {
	int* player_ids;
    struct tank** tanks;
    struct singly_linked_node* projectiles;
};

struct whole_world* whole_world_alloc();

void whole_world_set_values(struct whole_world* self, int* player_ids, struct tank** tanks, struct singly_linked_node* projectiles_in_game);

void whole_world_free(struct whole_world* self);

#endif