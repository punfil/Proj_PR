#ifndef TANK_H
#define TANK_H

struct tank;

struct tank* tank_alloc();

void tank_set_values(struct tank* self, uint32_t player_id, uint32_t x, uint32_t y, float angle, float hp);

void tank_free(struct tank *self);

#endif