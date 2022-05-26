#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "other_methods.h"

int* generate_id_list(int count){
    int* array = (int*)malloc(count*sizeof(int));
    if (array == NULL){
        return NULL;
    }
    for (int i=0;i<count;i++){
        array[i] = i;
    }
    return array;
}

int return_free_id(int* all_ids){
    for (int i=0;i<MAX_PLAYERS;i++){
        if (all_ids[i] != USED_ID){
            all_ids[i] = USED_ID;
            return i;
        }
    }
    return USED_ID;
}

void set_id_available(int* all_ids, int which){
    all_ids[which] = which;
}

void increment_players_count(int* players_count){
    ++(*players_count);
}

void decrement_players_count(int* players_count){
    --(*players_count);
}