#include <stdio.h>
#include <stdlib.h>

#include "singly_linked_list.h"

void singly_linked_list_add(struct singly_linked_node** head, void* data){
    struct singly_linked_node* temp = (struct singly_linked_node*)malloc(sizeof(struct singly_linked_node));
    if (temp == NULL){
        printf("Error allocating memory\n");
        return;
    }
    temp->data = data;
    //If the list is still not initialized
    if (*head == NULL){
        *head = temp;
        return;
    }
    struct singly_linked_node* iterator = *head;
    while (iterator->next != NULL){
        iterator = iterator->next;
    }
    iterator->next = temp;
}

//Probably not working properly, but no need to fix this for now. ** to head
void singly_linked_list_remove_at(struct singly_linked_node** head, int element_number){
    //There are no detection of errors!
    struct singly_linked_node* iterator = *head;
    for (int i=0;i<element_number-1 && iterator->next != NULL;i++){
        iterator = iterator->next;
    }
    struct singly_linked_node* temp = iterator->next;
    iterator->next = iterator->next->next;
    if (*head==temp){
        *head = NULL;
    }
    free(temp);
}

int singly_linked_list_count_elements(struct singly_linked_node* head){
    struct singly_linked_node* iterator = head;
    int counter = 0;
    while (iterator!=NULL){
        counter+=1;
        iterator = iterator->next;
    }
    return counter;
}

void* singly_linked_list_get_at(struct singly_linked_node* head, int element_number){
    //There is no detection of errors!
    struct singly_linked_node* iterator = head;
    for (int i=0;i<element_number-1 && iterator->next != NULL;i++){
        iterator = iterator->next;
    }
    return iterator->next;
}
