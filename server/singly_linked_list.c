#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

void singly_linked_list_add(struct singly_linked_node** head, void* data){
    struct singly_linked_node* temp = (struct singly_linked_node*)malloc(sizeof(struct singly_linked_node));
    if (temp == NULL) {
        printf("Error allocating memory\n");
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
