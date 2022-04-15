#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "information.h"
  

struct information* information_alloc(){
	struct information* self;
	self = malloc(sizeof(struct information*));
	if (self==NULL){
		return NULL;
	}
	memset(self, 0, sizeof(struct information*));
	return self;
}

void information_set_values(struct information* self, char action, char type_of, uint32_t player_id, uint32_t x_location, uint32_t y_location, uint32_t owner_id, float angle, float hp){
	self->action = action;
	self->type_of = type_of;
	self->player_id = player_id;
	self->x_location = x_location;
	self->y_location = y_location;
	self->angle = angle;
	self->hp = hp;
}

void information_free(struct information* self){
	free(self);
}