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

void tank_set_values(struct tank* self, uint32_t player_id, uint32_t x, uint32_t y, float tank_angle, float hp, float turret_angle, uint32_t tank_version){
    self->player_id = player_id;
    self->x = x;
    self->y = y;
    self->tank_angle = tank_angle;
    self->hp = hp;
    self->turret_angle = turret_angle;
    self->tank_version = tank_version;
}

void tank_update(struct tank* self, uint32_t x, uint32_t y, float tank_angle, float hp, float turret_angle){
    self->x = x;
    self->y = y;
    self->tank_angle = tank_angle;
    self->hp = hp;
    self->turret_angle = turret_angle;
}

void tank_free(struct tank *self){
    free(self);
}

