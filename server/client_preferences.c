#include "client_preferences.h"

struct client_preferences* client_preferences_alloc(){
    struct client_preferences* self = (struct client_preferences*)malloc(sizeof(struct client_preferences));
    return self;
}

void client_preferences_set_values(struct client_preferences* self, uint32_t tank_version, float tank_max_hp){
    self->tank_version = tank_version;
    self->tank_max_hp = tank_max_hp;
}

void client_preferences_free(struct client_preferences* self){
    free(self);
}