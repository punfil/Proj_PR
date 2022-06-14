#ifndef OTHER_METHODS_H
#define OTHER_METHODS_H

int* generate_id_list(int count);
int return_free_id(int* all_ids);
void set_id_used(int id, int * all_ids);
void set_id_available(int* all_ids, int which);

void increment_players_count(int* players_count);
void decrement_players_count(int* players_count);

#endif
