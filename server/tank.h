#ifndef TANK_H
#define TANK_H

struct tank {
	uint32_t player_id;
	float x;
	float y;
	float tank_angle;
	float hp;
	float turret_angle;
	uint32_t tank_version;
	bool shield_active;
};

struct tank* tank_alloc();

void tank_set_values(struct tank* self, uint32_t player_id, float x, float y, float tank_angle, float hp, float turret_angle, uint32_t tank_version, bool shield_active);
void tank_update(struct tank* self, float x, float y, float tank_angle, float hp, float turret_angle, bool shield_active);

void tank_free(struct tank *self);

#endif