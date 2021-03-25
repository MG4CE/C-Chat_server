#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "../include/socket_server.h"
#include "../include/request_handler.h"

typedef struct {
    user_list_t *list;
    int fd;
}new_connection_t;

int main (int argc, char **argv) {
    int debug = 0;
    char *port = NULL;
    int index;
    int c;
    int new_client;

    server_socket_t server;


    //---------------------------- Parse Args ----------------------------
    opterr = 0;

    while ((c = getopt (argc, argv, "dp:")) != -1) {
        switch (c) {
            case 'd':
                debug = 1;
                break;
            case 'p':
                port = optarg;
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
                abort();
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
        fprintf(stderr, "ERROR: %s", errno);
    }

    if (listen_socket(&server) == -1) {
        fprintf(stderr, "ERROR: %s", errno);
    }

    //------------------------ Accept New Clients ------------------------
    int user_count;

    user_list_t *list = malloc(sizeof(user_list_t));

    while (new_client = accept_connection(&server)) {
        if (user_count >= USER_LIMIT) {
            //make this a template later
            message_t server_full;
            server_full.command = ERROR;
            strcpy(server_full.message, "SERVER FULL");
            send_message(new_client, &server_full, sizeof(message_t));
            close(new_client);
        }

        pthread_t client_thread;
        new_connection_t *new_user = malloc(sizeof(new_connection_t));
        new_user->fd = new_client;
        pthread_create(&client_thread, NULL, client_handler, (void *)new_user);
        user_count++;
    }

    if (new_client == -1) {
        fprintf(stderr, "ERROR: %s", errno);
    }

    return 0;
}

void * client_handler(void *args) {
    //TODO add check for no username requests
    //TODO add timeout process using select

    new_connection_t *info = (new_connection_t *)args;

    user_t *user = malloc(sizeof(user_t));
    user->user_descriptor = info->fd;

    add_user(info->list, user);

    fd_set event_fd, ready_fd;

    FD_ZERO(&event_fd);
    FD_SET(info->fd, &event_fd);

    while (1){
        ready_fd = event_fd;

        if (select(1, &ready_fd, NULL, NULL, NULL) < 0) {
            fprintf(stderr, "ERROR: %s, on thread fd_%i", errno, user->user_descriptor);
        }

        if (FD_ISSET(info->fd, &ready_fd)) {
            message_t msg;
            if (fetch_message(info->fd, &msg, sizeof(message_t)) == -1) {
                fprintf(stderr, "ERROR: %s", errno);
                close_connection(info->fd);
                break;
            } 
            if (process_request(&msg) == 0) break;
        }
    }
    remove_user(info->list, user);
    free(info);
    free(user);
}


int process_request(message_t *msg) {

}


