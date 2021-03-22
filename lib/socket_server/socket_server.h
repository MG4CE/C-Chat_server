#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H
#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAX_MESSAGE_LEN 4000
#define MAX_USERNAME_LEN 21

typedef enum{
    SET_USERNAME,
    GET_USERS,
    SEND_PUBLIC,
    SEND_PRIVATE,
    LEAVE,
    DISCONNECT,
    ERROR,
}request;

typedef struct message {
    request command;
    char message[MAX_MESSAGE_LEN];
    char selected_user[MAX_USERNAME_LEN];
}message_t;

typedef struct {
    int socket_descriptor;
    int port;
}server_socket_t;

int create_socket();
int bind_socket(server_socket_t *server);
int listen_socket(server_socket_t *server);
int accept_connection(server_socket_t *server_info);
int send_message(int client_fd, message_t *message, size_t message_size);
int fetch_message(int client_fd, message_t *message, size_t message_size);
#endif
