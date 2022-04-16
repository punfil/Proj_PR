#ifndef TANK_H
#define TANK_H

struct tank {
	uint32_t player_id;
	uint32_t x;
	uint32_t y;
	float tank_angle;
	float hp;
	float turret_angle;
	uint32_t tank_version;
};

struct tank* tank_alloc();

void tank_set_values(struct tank* self, uint32_t player_id, uint32_t x, uint32_t y, float tank_angle, float hp, float turret_angle, uint32_t tank_version);
void tank_update(struct tank* self, uint32_t x, uint32_t y, float tank_angle, float hp, float turret_angle);

void tank_free(struct tank *self);

#endif