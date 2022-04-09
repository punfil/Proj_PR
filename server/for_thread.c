#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>

#include "for_thread.h"

struct for_thread {
	uint32_t player_id;
	uint32_t players_count;
	int* csocket;
	struct sockaddr_in* client;
};

struct for_thread* for_thread_alloc(){
    struct for_thread* self;
    self = malloc(sizeof(struct for_thread*));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct for_thread*));
    return self;
}

void for_thread_set_values(struct for_thread* self, uint32_t player_id, uint32_t players_count, int* csocket, struct sockaddr_in* client){
    self->player_id = player_id;
    self->players_count = players_count;
    self->csocket = csocket;
    self->client = client;
}

void for_thread_free(struct for_thread* self){
    free(self);
}