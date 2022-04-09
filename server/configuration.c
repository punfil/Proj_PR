#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "information.h"

struct configuration{
	uint32_t width;
	uint32_t height;
	uint32_t background_scale;
	uint32_t player_count;
	uint32_t player_id;
	uint32_t tank_spawn_x;
	uint32_t tank_spawn_y;
};

struct configuration* configuration_alloc(){
    struct configuration* self;
    self = malloc(sizeof(struct configuration*));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct configuration*));
    return self;
}

void configuration_set_values(struct configuration* self, uint32_t width, uint32_t height, uint32_t background_scale, uint32_t player_count, uint32_t player_id, uint32_t tank_spawn_x,uint32_t tank_spawn_y){
    self->width = width;
    self->height = height;
    self->background_scale = background_scale;
    self->player_count = player_count;
    self->player_id = player_id;
    self->tank_spawn_x = tank_spawn_x;
    self->tank_spawn_y = tank_spawn_y;
}

void configuration_free(struct configuration* self){
    free(self);
}