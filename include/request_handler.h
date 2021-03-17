#ifndef REQUESTS_H
#define REQUESTS_H

#include "../lib/user_manager/user_manager.h"

typedef enum{
    CONNECT,
    SET_USERNAME,
    GET_USERS,
    SEND_PUBLIC,
    SEND_PRIVATE,
    REQUEST_SESSION,
    LEAVE,
    DISCONNECT,
    ERROR,
}request;

int client_connect();
int check_username();
char get_available_users();
int send_chat_request();
int send_message();
int send_message_public();
int leave_chat_session();
int client_disconnect();

#endif