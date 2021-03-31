#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/poll.h>

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
        fprintf(stderr, "ERROR: Invalid Port!\n");
        exit(-1);
    }

    if (bind_socket(&server) == -1) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(-1);
    }

    if (listen_socket(&server) == -1) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(-1);
    }

    puts("Server Started!");

    //------------------------ Accept New Clients ------------------------
    int user_count;

    user_list_t *list = malloc(sizeof(user_list_t));

    while ((new_client = accept_connection(&server)) != -1) {
        printf("%d\n", new_client);
        if (user_count >= USER_LIMIT) {
            message_t server_full;
            server_full.command = ERROR;
            strcpy(server_full.message, "SERVER FULL");
            if (send_message(new_client, &server_full, sizeof(message_t))){
                fprintf(stderr, "ERROR: %s\n", strerror(errno));
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
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(-1);
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

    //issue where messages are spammed when client killed while recv is blocking

    struct pollfd *fds = malloc(sizeof(struct pollfd));

    fds->fd = user->user_descriptor;
    fds->events = POLLIN;

    while (1) {
        int ret = poll(fds, 1, -1);

        if (ret == -1) {
            perror ("poll");
            break;
        }

        if (!ret) {
            printf ("%d seconds elapsed.\n", 10);
            break;
        }

        if (fds->revents & POLLIN) {
            message_t msg;
            if (fetch_message(info->fd, &msg, sizeof(message_t)) == -1) {
                fprintf(stderr, "ERROR: %s\n", strerror(errno));
                close_connection(info->fd);
                break;
            }

            //mutex lock here\n
            if (process_request(info->list, user, &msg) == -1) {
                //mutex unlock here
                break;
            }
            //mutex unlock here
        }
    }   

    remove_user(info->list, user);
    free(info);
    free(fds);
    return NULL;
}

int process_request(user_list_t *list, user_t *user, message_t *msg) {
    if (msg->command == SEND_PUBLIC) {
        return send_message_public(list, user, msg);
    } else if (msg->command == SEND_PRIVATE) {
        return send_message_private(list, user, msg);
    } else if (msg->command == SET_USERNAME) {
        set_username(list, user, msg->selected_user);
        message_t msg_list;
        msg_list.command = SUCCESS;
        return send_message(user->user_descriptor, &msg_list, sizeof(message_t));
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
 