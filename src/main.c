#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "../include/socket_server.h"
#include "../include/request_handler.h"

#define MAX_PORT_LEN 5

typedef struct {
    user_list_t *list;
    int fd;
}new_connection_t;

void * client_handler(void *args);
int process_request(user_list_t *list, user_t *user, message_t *msg) ;

int main (int argc, char **argv) {
    char port[MAX_PORT_LEN] = "80";
    int index;
    int c;
    int new_client;

    server_socket_t server;

    //---------------------------- Parse Args ----------------------------
    opterr = 0;

    while ((c = getopt (argc, argv, "p:")) != -1) {
        switch (c) {
            case 'p':
                if (strlen(optarg) > MAX_PORT_LEN) {
                    fprintf(stderr, "Invalid port.\n");
                    exit(-1);
                }
                strcpy(port, optarg);
                break;
            case '?':
                if (optopt == 'p') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                exit(-1);
        }
    } 

    for (index = optind; index < argc; index++) {
        printf ("Non-option argument %s\n", argv[index]);
    }

    //----------------------- Server Socket Setup -----------------------
    server.socket_descriptor = create_socket();

    server.port = atoi(port);
    if (server.port == 0) {
        fprintf(stderr, "ERROR: Invalid Port!");
        exit(-1);
    }

    if (bind_socket(&server) == -1) {
        fprintf(stderr, "ERROR: %s", strerror(errno));
    }

    if (listen_socket(&server) == -1) {
        fprintf(stderr, "ERROR: %s", strerror(errno));
    }

    puts("Server Started!");

    //------------------------ Accept New Clients ------------------------
    int user_count;

    user_list_t *list = malloc(sizeof(user_list_t));

    while ((new_client = accept_connection(&server)) != -1) {
        if (user_count >= USER_LIMIT) {
            message_t server_full;
            server_full.command = ERROR;
            strcpy(server_full.message, "SERVER FULL");
            if (send_message(new_client, &server_full, sizeof(message_t))){
                fprintf(stderr, "ERROR: %s", strerror(errno));
            }
            close(new_client);
        }

        pthread_t client_thread;
        new_connection_t *new_user = malloc(sizeof(new_connection_t));
        new_user->fd = new_client;
        new_user->list = list;
        pthread_create(&client_thread, NULL, client_handler, (void *)new_user);
        user_count++;
    }

    if (new_client == -1) {
        fprintf(stderr, "ERROR: %s", strerror(errno));
    }

    return 0;
}

void * client_handler(void *args) {
    //TODO add check for no username set requests
    //TODO add timeout process using select

    new_connection_t *info = (new_connection_t *)args;

    user_t *user = malloc(sizeof(user_t));
    user->user_descriptor = info->fd;

    //mutex lock here
    add_user(info->list, user);

    fd_set event_fd, ready_fd;

    FD_ZERO(&event_fd);
    FD_SET(info->fd, &event_fd);

    while (1){
        ready_fd = event_fd;

        //add timeout
        if (select(1, &ready_fd, NULL, NULL, NULL) == -1) {
            fprintf(stderr, "ERROR: %s, on thread fd_%i", strerror(errno), user->user_descriptor);
        }

        if (FD_ISSET(info->fd, &ready_fd)) {
            message_t msg;
            if (fetch_message(info->fd, &msg, sizeof(message_t)) == -1) {
                fprintf(stderr, "ERROR: %s", strerror(errno));
                close_connection(info->fd);
                break;
            } 
            //mutex lock here
            if (process_request(info->list, user, &msg) == -1) {
                //mutex unlock here
                break;
            }
            //mutex unlock here
        }
    }
    remove_user(info->list, user);
    free(info);
    free(user);

    return 0;
}

int process_request(user_list_t *list, user_t *user, message_t *msg) {
    if (msg->command == SEND_PUBLIC) {
        return send_message_public(list, user, msg);
    } else if (msg->command == SEND_PRIVATE) {
        return send_message_private(list, user, msg);
    } else if (msg->command == SET_USERNAME) {
        return set_username(list, user, msg->selected_user);
    } else if (msg->command == GET_USERS) {
        message_t msg_list;
        msg_list.command = GET_USERS;
        strcpy(msg_list.message, get_users_list(list));
        return send_message(user->user_descriptor, &msg_list, sizeof(message_t));
    } else if (msg->command == DISCONNECT) {
        return client_disconnect(list, user);
    } else {
        message_t err_msg;
        err_msg.command = ERROR;
        strcpy(err_msg.message, "Unknown command!");
        return send_message(user->user_descriptor, &err_msg, sizeof(message_t));
    }
}
 