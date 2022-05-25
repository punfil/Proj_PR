#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

void singly_linked_list_add(struct singly_linked_node** head, void* data){
    struct singly_linked_node* temp = (struct singly_linked_node*)malloc(sizeof(struct singly_linked_node));
    if (temp == NULL) {
        printf("##ERROR: Error allocating memory\n");
        return;
    }
    temp->data = data;
    temp->next = NULL;
    //If the list is still not initialized
    if (*head == NULL) {
        *head = temp;
        return;
    }
    struct singly_linked_node* iterator = *head;
    while (iterator->next != NULL) {
        iterator = iterator->next;
    }
    iterator->next = temp;
}

void* singly_linked_list_get_at(struct singly_linked_node* head, int index){
    struct singly_linked_node* iterator = head;
    for (int i=0; i < index ;i++){
        if (iterator == NULL){
            return NULL;
        }
        iterator = iterator->next;
    }
    if (iterator == NULL){
        return NULL;
    }
    return iterator->data;
}
