#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

struct singly_linked_node{
    void* data;
    struct singly_linked_node* next;
};

void singly_linked_list_add(struct singly_linked_node** head, void* data); 
void* singly_linked_list_get_at(struct singly_linked_node* head, int index);

#endif