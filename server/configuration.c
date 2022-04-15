#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "configuration.h"

struct configuration* configuration_alloc(){
    struct configuration* self;
    self = malloc(sizeof(struct configuration*));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct configuration*));
    return self;
}

void configuration_set_values(struct configuration* self, uint32_t width, uint32_t height, uint32_t background_scale, uint32_t players_count, uint32_t player_id, uint32_t tank_spawn_x,uint32_t tank_spawn_y, uint32_t map_number){
    self->width = width;
    self->height = height;
    self->background_scale = background_scale;
    self->players_count = players_count;
    self->player_id = player_id;
    self->tank_spawn_x = tank_spawn_x;
    self->tank_spawn_y = tank_spawn_y;
    self->map_number = map_number;
}

void configuration_update_values(struct configuration* self, uint32_t player_id, uint32_t players_count){
    self->player_id = player_id;
    self->players_count = players_count;
}

void configuration_free(struct configuration* self){
    free(self);
}