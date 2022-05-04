#ifndef INFORMATION_H
#define INFORMATION_H

struct information {
	char action; //Update = movement, create = spawn new tank
	char type_of; //Projectile or Tank
	uint32_t player_id; //Player id that this action involves
	uint32_t x_location; //Location of x
	uint32_t y_location; //Location of y
	float tank_angle; //Angle - for tank turret
	float hp; //For tanks
	float turret_angle;
 };

struct information* information_alloc();

void information_set_values(struct information* self, char action, char type_of, uint32_t player_id, uint32_t x_location, uint32_t y_location, float tank_angle, float hp, float turret_angle);

void information_free(struct information* self);

#endif