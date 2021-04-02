#include "../include/request_handler.h"

int client_connect(user_list_t *connected_users, int client_fd){
    user_t *new_user = malloc(sizeof(user_t));
    new_user->user_descriptor = client_fd;
    if (get_user_fd(connected_users, client_fd) == NULL) {
        add_user(connected_users, new_user);
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
        strcat(message, " ");
        temp = temp->next;
    }
    return message;
}


int send_message_private(user_list_t *connected_users, user_t *sender, message_t *message) {
    if (message->command != SEND_PRIVATE) {
        return -3;
    }
    user_t *target = get_user_username(connected_users, message->selected_user);
    if (target == NULL) {
        return -2;
    }
    message_t forward;
    forward.command = SEND_PRIVATE;
    strcpy(forward.message, message->message);
    strcpy(forward.selected_user, sender->username);

    if (send_message(target->user_descriptor, &forward, sizeof(message_t)) == -1){
        return -1;
    }
    return 1;
}

int send_message_public(user_list_t *connected_users, user_t *sender, message_t *message){
    if (message->command != SEND_PUBLIC) {
        return -1;
    }

    strcpy(message->selected_user, sender->username);

    user_t *curr_user = connected_users->head;

    while (curr_user != NULL) {
        if (curr_user == sender) {
            curr_user = curr_user->next;
            continue;
        }
        
        if (send_message(curr_user->user_descriptor, message, sizeof(message_t)) == -1){
            fprintf(stderr, "ERROR: Failed to send to %s, closing connection.", curr_user->username);
            client_disconnect(connected_users, curr_user);
            remove_user(connected_users, curr_user);
        }
        curr_user = curr_user->next;
    }
    return 1;
}

int client_disconnect(user_list_t *connected_users, user_t *user) {
    //TODO inform all users of disconnect
    if (remove_user(connected_users, user) == 0){
        return 0;
    }

    if (close_connection(user->user_descriptor) == -1) {
        return -1;
    }

    return 1;
}
