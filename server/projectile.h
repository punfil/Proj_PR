#ifndef PROJECTILE_H
#define PROJECTILE_H

#ifndef SINGLY_LINKED_LIST_H
#include "singly_linked_list.h"
#endif

struct projectile{
    uint32_t id;
    uint32_t owner_id;
    uint32_t x;
    uint32_t y;
    float angle;
    uint32_t hp;
};

struct projectile* projectile_alloc();

void projectile_set_values(struct projectile* self, uint32_t id, uint32_t owner_id, uint32_t x, uint32_t y, float angle, float hp);

bool projectile_with_id_exists(struct singly_linked_node* head, int projectile_id);

void remove_projectile_from_list(struct singly_linked_node** head, int projectile_id);

void update_projectile_values(struct singly_linked_node* head, int projectile_id, int x_location, int y_location);

void projectile_free(struct projectile *self);

#endif