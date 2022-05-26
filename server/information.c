#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "information.h"
  

struct information* information_alloc(){
	struct information* self;
	self = (struct information*)malloc(sizeof(struct information));
	if (self==NULL){
		return NULL;
	}
	memset(self, 0, sizeof(struct information*));
	return self;
}

void information_set_values(struct information* self, char action, char type_of, uint32_t player_id, float x_location, float y_location, float tank_angle, float hp, float turret_angle){
	self->action = action;
	self->type_of = type_of;
	self->player_id = player_id;
	self->x_location = x_location;
	self->y_location = y_location;
	self->tank_angle = tank_angle;
	self->hp = hp;
	self->turret_angle = turret_angle;
}

void information_free(struct information* self){
	free(self);
}