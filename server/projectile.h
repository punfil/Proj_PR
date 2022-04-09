#ifndef PROJECTILE_H
#define PROJECTILE_H

struct projectile;

struct projectile* projectile_alloc();

void set_values(struct projectile* self, uint32_t owner_id, uint32_t x, uint32_t y, float angle, float hp);

void projectile_free(struct projectile *self);

#endif