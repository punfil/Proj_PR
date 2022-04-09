#ifndef INFORMATION_H
#define INFORMATION_H

struct information;

struct information* information_alloc();

void information_set_values(struct information* self, char action, char type_of, uint32_t player_id, uint32_t x_location, uint32_t y_location, uint32_t owner_id, float angle, float hp);

void information_free(struct information* self);

#endif