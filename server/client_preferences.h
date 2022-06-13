#ifndef CLIENT_PREFERENCES_H
#define CLIENT_PREFERENCES_H
#include <stdint.h>
#include <stdlib.h>

struct client_preferences{
	uint32_t tank_version;
    float tank_max_hp;
};

struct client_preferences* client_preferences_alloc();

void client_preferences_set_values(struct client_preferences* self, uint32_t tank_version, float tank_max_hp);

void client_preferences_free(struct client_preferences* self);

#endif