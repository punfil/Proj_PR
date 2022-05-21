#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "projectile.h"


struct projectile* projectile_alloc(){
    struct projectile* self;
    self = (struct projectile*)malloc(sizeof(struct projectile));
    if (self == NULL){
        return NULL;
    }
    memset(self, 0, sizeof(struct projectile*));
    return self;
}

void projectile_set_values(struct projectile* self, uint32_t id, uint32_t owner_id, uint32_t x, uint32_t y, float angle, float hp){
    self->id = id;
    self->owner_id = owner_id;
    self->x = x;
    self->y = y;
    self->angle = angle;
    self->hp = hp;
}

bool projectile_with_id_exists(struct singly_linked_node* head, int projectile_id){
    struct singly_linked_node* iterator = head;
    while (iterator != NULL){
        if (((struct projectile*)iterator->data)->id == projectile_id){
            return true;
        }
        iterator = iterator->next;
    }
    return false;
}

//This makes a very serious assumption - that this list works as FIFO
void remove_projectile_from_list(struct singly_linked_node** head, int projectile_id){
    printf("Received task to remove projectile ID %d\n", projectile_id);
    projectile_free((struct projectile*)(*head)->data);
    if ((*head)->next == NULL){
        free(*head);
        *head = NULL;
        return;
    }
    else{
        struct singly_linked_node* temp = *head;
        *head = (*head)->next;
        free(temp);
    }

    // struct singly_linked_node* iterator = *head;
    // while (iterator->next != NULL && iterator->next->next != NULL && ((struct projectile*)iterator->next->data)->id != projectile_id){
    //     iterator = iterator->next;
    // }
    // //If lists consists of one element
    // if (iterator == *head && iterator!=NULL){
    //     projectile_free((struct projectile*)iterator->data);
    //     free(iterator);
    //     *head = NULL;
    //     return;
    // }
    // //If it's the last element in the list
    // else if(iterator->next != NULL){
    //     projectile_free((struct projectile*)iterator->next->data);
    //     free(iterator);
    //     iterator->next = NULL;
    // }    
    // else{
    //     struct singly_linked_node* temp = iterator->next;
    //     iterator->next = iterator->next->next;
    //     projectile_free((struct projectile*)iterator->next->data);
    //     free(temp);
    // }
}

void update_projectile_values(struct singly_linked_node* head, int projectile_id, int x_location, int y_location){
    struct singly_linked_node* iterator = head;
    while (iterator != NULL){
        if (((struct projectile*)iterator->data)->id == projectile_id){
            break;
        }
        iterator = iterator->next;
    }
    struct projectile* this_projectile = (struct projectile*)iterator->data;
    this_projectile->x = x_location;
    this_projectile->y = y_location;
}

void projectile_free(struct projectile *self){
    free(self);
}
