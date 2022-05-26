#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "projectile.h"


struct projectile* projectile_alloc(){
    struct projectile* self;
    self = (struct projectile*)malloc(sizeof(struct projectile));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct projectile*));
    return self;
}

void projectile_set_values(struct projectile* self, uint32_t id, uint32_t owner_id, uint32_t x, uint32_t y, float angle, float hp){
    self->id = id;
    self->owner_id = owner_id;
    self->x = x;
    self->y = y;
    self->angle = angle;
    self->hp = hp;
}

//Return projectile if exists or NULL if doesn't
struct projectile* get_projectile_with_id(struct projectile** head, int projectile_id){
    return head[projectile_id];
}

void remove_projectile_from_list(struct projectile** head, int projectile_id){
    free(head[projectile_id]);
    head[projectile_id] = NULL;
}

void update_projectile_values(struct projectile** head, int projectile_id, int x_location, int y_location){
    struct projectile* projectile = get_projectile_with_id(head, projectile_id);
    if (projectile!=NULL){
        projectile->x = x_location;
        projectile->y = y_location;
    }
}

void projectile_free(struct projectile *self){
    free(self);
}
