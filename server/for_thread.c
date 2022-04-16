#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

//Networking
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "for_thread.h"


struct for_thread* for_thread_alloc(){
    struct for_thread* self;
    self = (struct for_thread*)malloc(sizeof(struct for_thread));
    self->running = (bool*)malloc(sizeof(bool));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct for_thread*));
    return self;
}

void for_thread_set_values(struct for_thread* self, int* player_ids, struct tank** tanks_in_game, struct singly_linked_node* projectiles_in_game, int** csockets, struct sockaddr_in** clients){
    self->player_ids = player_ids;
    self->tanks_in_game = tanks_in_game;
    self->projectiles_in_game = projectiles_in_game;
    self->csockets = csockets;
    self->clients = clients;
}

void for_thread_free(struct for_thread* self){
    free(self->running);
    free(self);
}