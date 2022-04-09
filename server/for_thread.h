#ifndef FOR_THREAD_H
#define FOR_THREAD_H

struct for_thread;

struct for_thread* for_thread_alloc();

void for_thread_set_values(struct for_thread* self, uint32_t player_id, uint32_t players_count, int* csocket, struct sockaddr_in* client);

void for_thread_free(struct for_thread* self);

#endif

