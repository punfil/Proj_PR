#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "whole_world.h"

struct whole_world* whole_world_alloc(){
    struct whole_world* self;
    self = (struct whole_world*)malloc(sizeof(struct whole_world));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct whole_world*));
    return self;
}

void whole_world_set_values(struct whole_world* self, int* players_ids, struct tank** tanks, struct singly_linked_node** projectiles_in_game){
    self->player_ids = players_ids;
    self->tanks = tanks;
    self->projectiles = projectiles_in_game;
}

void whole_world_free(struct whole_world* self){
    free(self);
}