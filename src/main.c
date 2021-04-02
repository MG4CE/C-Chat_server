#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/poll.h>

#include "../include/socket_server.h"
#include "../include/request_handler.h"

//TODO fix address alrady in use error after quick start

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

    //TODO Fix issue where messages are spammed and server crashes when client is killed

    struct pollfd *fds = malloc(sizeof(struct pollfd));

    fds->fd = user->user_descriptor;
    fds->events = POLLIN;

    while (1) {
        int ret = poll(fds, 1, -1);

        if (ret == -1) {
            fprintf(stderr, "ERROR: %s", strerror(errno));
            break;
        }

        if (!ret) {
            puts("Connection Timedout!");
            break;
        }

        if (fds->revents & POLLIN) {
            message_t msg;
            if (fetch_message(info->fd, &msg, sizeof(message_t)) == -1) {
                fprintf(stderr, "ERROR: %s\n", strerror(errno));
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
    free(fds);
    return NULL;
}

int process_request(user_list_t *list, user_t *user, message_t *message) {
    int ret = 0;
    message_t msg;

    switch (message->command) {   
        case SEND_PUBLIC:
            ret = send_message_public(list, user, message);
            break;
        case SEND_PRIVATE:
            ret = send_message_private(list, user, message);
            break;
        case SET_USERNAME:
            set_username(list, user, message->selected_user);
            msg.command = SUCCESS;
            ret = send_message(user->user_descriptor, &msg, sizeof(message_t));
            break;
        case GET_USERS:
            msg.command = GET_USERS;
            strcpy(msg.message, get_users_list(list));
            ret = send_message(user->user_descriptor, &msg, sizeof(message_t));
            break;
        default:
            msg.command = ERROR;
            strcpy(msg.message, "Unknown command!");
            ret = send_message(user->user_descriptor, &msg, sizeof(message_t));
            break;
    }
    return ret;
}

 