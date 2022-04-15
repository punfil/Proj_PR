#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "projectile.h"

struct projectile{
    uint32_t owner_id;
    uint32_t x;
    uint32_t y;
    float angle;
    uint32_t hp;
};

struct projectile* projectile_alloc(){
    struct projectile* self;
    self = malloc(sizeof(struct projectile*));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct projectile*));
    return self;
}

void set_values(struct projectile* self, uint32_t owner_id, uint32_t x, uint32_t y, float angle, float hp){
    self->owner_id = owner_id;
    self->x = x;
    self->y = y;
    self->angle = angle;
    self->hp = hp;
}

void projectile_free(struct projectile *self){
    free(self);
}
