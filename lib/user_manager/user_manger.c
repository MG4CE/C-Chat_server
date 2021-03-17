#include "user_manager.h"

//Integrity check for head and tail? (could cause memory leak)

void add_user(user_list_t *list, user_t *node) {
    if (list == NULL || (list->head == NULL && list->tail == NULL)) {
        list->head = node;
        list->tail = node;
        node->next = NULL;
        return;
    }
    
    list->tail->next = node;
    list->tail = node;
    node->next = NULL;
}

void remove_user(user_list_t *list, user_t *node) {
    if (list == NULL || (list->head == NULL && list->tail == NULL)) {
        return;
    }

    if (list->head == node && list->head == list->tail) {
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        node = NULL;
        return;
    }

    user_t *temp = list->head->next;
    user_t *prev = list->head;

    while (temp != NULL) {
        if (temp == node) {
            if (temp == list->tail) {
                list->tail = prev;
            }
            prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

user_t * get_user(user_list_t *list, char *username) {
    if (list == NULL || (list->head == NULL && list->tail == NULL)) {
        return NULL;
    }

    user_t *temp = list->head;

    while(temp != NULL) {
        if(temp->username == username) { 
            return temp;                
        }
        temp = temp->next;
    }  
    return NULL;
}
