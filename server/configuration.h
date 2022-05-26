#ifndef CONFIGURATION_H
#define CONFIGURATION_H

struct configuration{
	uint32_t width;
	uint32_t height;
	uint32_t background_scale;
	uint32_t players_count;
	uint32_t player_id;
	float tank_spawn_x;
	float tank_spawn_y;
    uint32_t map_number;
};

struct configuration* configuration_alloc();

void configuration_set_values(struct configuration* self, uint32_t width, uint32_t height, uint32_t background_scale, uint32_t players_count, uint32_t player_id, float tank_spawn_x, float tank_spawn_y, uint32_t map_number);
void configuration_update_values(struct configuration* self, uint32_t player_id, uint32_t players_count);

void configuration_free(struct configuration* self);

#endif
