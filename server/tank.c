#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "tank.h"


struct tank* tank_alloc(){
    struct tank* self;
    self = malloc(sizeof(struct tank*));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct tank*));
    return self;
}

void tank_set_values(struct tank* self, uint32_t player_id, uint32_t x, uint32_t y, float angle, float hp){
    self->player_id = player_id;
    self->x = x;
    self->y = y;
    self->angle = angle;
    self->hp = hp;
}

void tank_free(struct tank *self){
    free(self);
}

