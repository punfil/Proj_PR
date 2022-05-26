#ifndef PROJECTILE_H
#define PROJECTILE_H

#ifndef SINGLY_LINKED_LIST_H
#include "singly_linked_list.h"
#endif

struct projectile{
    uint32_t id;
    uint32_t owner_id;
    float x;
    float y;
    float angle;
    uint32_t hp;
};

struct projectile* projectile_alloc();

void projectile_set_values(struct projectile* self, uint32_t id, uint32_t owner_id, float x, float y, float angle, float hp);

struct projectile* get_projectile_with_id(struct projectile** head, int projectile_id);

void remove_projectile_from_list(struct projectile** head, int projectile_id);

void update_projectile_values(struct projectile** head, int projectile_id, float x_location, float y_location);

void projectile_free(struct projectile *self);

#endif