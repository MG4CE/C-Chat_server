#ifndef REQUESTS_H
#define REQUESTS_H

#include "../lib/user_manager/user_manager.h"
#include "../lib/socket_server/socket_server.h"
#include <string.h>

int client_connect(user_list_t *connected_users, int client_fd);
int set_username(user_list_t *connected_users, user_t *user, char *username);
char * get_users_list(user_list_t *connected_users);
int send_message_private(user_list_t *connected_users, user_t *sender, message_t *message);
int send_message_public(user_list_t *connected_users, user_t *sender, message_t *message);
int client_disconnect(user_list_t *connected_users, user_t *user);
#endif