#ifndef USER_MANAGER_H 
#define USER_MANAGER_H 
#include <stddef.h>
#include <stdbool.h>    

#define MAX_USERNAME_LEN 21
#define USER_LIMIT 100

typedef enum {
    PRIVATE,
    PUBLIC,
}session;

typedef struct connected_user {
    char username[MAX_USERNAME_LEN];
    int user_descriptor;
    struct connected_user *next;
}user_t;

typedef struct connected_user_list {
    user_t *head;
    user_t *tail;
}user_list_t;

void add_user(user_list_t *list, user_t *node);
int remove_user(user_list_t *list, user_t *node);
user_t * get_user_username(user_list_t *list, char *username);
user_t * get_user_fd(user_list_t *list, int fd);
#endif
