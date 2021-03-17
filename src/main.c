//multithreaded server:
//parse args (port) X
//setup X
//bind X 
//listen X
//loop forever and accept clients, main will just throw clients in thread and deal with them
    //all communcation between client will be done in thread
    //each client will live in a thread
    //allow client intractions
    //relay messages between users
    //timeout process

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "../lib/socket_server/socket_server.h"

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
    while (new_client = accept_connection(&server) != -1) {
        //threading setup
    }

    if (new_client == -1) {
        fprintf(stderr, "ERROR: %s", errno);
    }

    return 0;
}

void *client_handler(void *args) {

}