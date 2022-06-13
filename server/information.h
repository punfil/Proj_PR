#ifndef INFORMATION_H
#define INFORMATION_H
#include <stdint.h>
#include <stdbool.h>

struct information {
	char action; //Update = movement, create = spawn new tank
	char type_of; //Projectile or Tank
	uint32_t player_id; //Player id that this action involves
	float x_location; //Location of x
	float y_location; //Location of y
	float tank_angle; //Angle - for tank turret
	float hp; //For tanks
	float turret_angle;
	uint32_t tank_version;
	bool shield_active;
 };

struct information* information_alloc();

void information_set_values(struct information* self, char action, char type_of, uint32_t player_id, float x_location, float y_location, float tank_angle, float hp, float turret_angle, uint32_t tank_version, bool shield_active);

void information_free(struct information* self);

#endif