#include "../include/request_handler.h"

int client_connect(user_list_t *connected_users, int client_fd){
    if (get_user_fd(connected_users, client_fd) == NULL) {
        add_user(connected_users, client_fd);
        return 1;
    }
    return -1;
}

int set_username(user_list_t *connected_users, user_t *user, char *username) {
    size_t username_len = strlen(username);

    if (username_len > 21) {
        return -3;
    } else if (username_len == 0) {
        return -2;
    }

    if (get_user_username(connected_users, username) != NULL) {
        return -1;
    }

    strcpy(user->username, username);
    return 1;
}

char * get_users_list(user_list_t *connected_users){
    char *message = malloc(MAX_MESSAGE_LEN * sizeof(char));

    user_t *temp = connected_users->head;

    if (temp == NULL) {
        message = "NO CONNECTED USERS";
    }

    while (temp != NULL) {
        //TODO add size safety check just incase
        strcat(message, temp->username);
        temp = temp->next;
    }
    return message;
}


int send_message_private(user_list_t *connected_users, user_t *sender, message_t *message) {
    if (message->command != PRIVATE) {
        return -3;
    }
    user_t *target = get_user_username(connected_users, message->selected_user);
    if (target == NULL) {
        return -2;
    }
    message_t forward;
    forward.command = PRIVATE;
    strcpy(forward.message, message->message);
    strcpy(forward.selected_user, sender->username);

    if (send_message(target->user_descriptor, &forward, sizeof(message_t)) == -1){
        return -1;
    }

    return 1;
}

int send_message_public(user_list_t *connected_users, user_t *sender, message_t *message){
    if (message->command != PUBLIC) {
        return -1;
    }

    user_t *curr_user = connected_users->head;

    while (curr_user != NULL) {
        if (send_message(curr_user->user_descriptor, message, sizeof(message_t)) == -1){
            fprintf(stderr, "ERROR: Failed to send to %s",curr_user->username);
            //maybe disconnect client here?
        }
    }

    return 1;
}

int client_disconnect(user_list_t *connected_users, user_t *user) {
    if (remove_user(connected_users, user) == 0){
        return 0;
    }
    //add connection close call when ready
    return 1;
}
