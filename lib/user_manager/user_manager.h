#ifndef USER_MANAGER_H 
#define USER_MANAGER_H 
#include <stddef.h>
#include <stdbool.h>    

typedef struct connected_user {
    char *username;
    int user_descriptor;
    bool is_connected;
    bool is_available;
    struct connected_user *next;
}user_t;

typedef struct connected_user_list {
    user_t *head;
    user_t *tail;
}user_list_t;

void add_user(user_list_t *list, user_t *node);
void remove_user(user_list_t *list, user_t *node);
user_t * get_user(user_list_t *list, char *username);
#endif
