#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H
#include<stdio.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

#define DEFAULT_MESSAGE_SIZE 1000

typedef struct {
    int socket_descriptor;
    int port;
}server_socket_t;

int create_socket();
int bind_socket(server_socket_t *server);
int listen_socket(server_socket_t *server);
int accept_connection(server_socket_t *server_info);
int send_message(int client_fd, char *message, size_t message_size);
int fetch_message(int client_fd, char *message, size_t message_size); 
#endif
