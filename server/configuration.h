#ifndef CONFIGURATION_H
#define CONFIGURATION_H

struct configuration;

struct configuration* configuration_alloc();

void configuration_set_values(struct configuration* self, uint32_t width, uint32_t height, uint32_t background_scale, uint32_t players_count, uint32_t player_id, uint32_t tank_spawn_x,uint32_t tank_spawn_y);
void configuration_set_values(struct configuration* self, uint32_t player_id, uint32_t players_count);

void configuration_free(struct configuration* self);

#endif
