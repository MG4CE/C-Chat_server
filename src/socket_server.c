#include "socket_server.h"

int create_socket() {
	return socket(AF_INET , SOCK_STREAM , 0);
}

int bind_socket(server_socket_t *server) {
	struct sockaddr_in server_access_settings;
	server_access_settings.sin_family = AF_INET;
	server_access_settings.sin_addr.s_addr = INADDR_ANY;
	server_access_settings.sin_port = htons(server->port);
	return bind(server->socket_descriptor,(struct sockaddr *)&server , sizeof(server));
}

int listen_socket(server_socket_t *server) {
	return listen(server->socket_descriptor , BACKLOG_LIMIT);
}

int accept_connection(server_socket_t *server_info) {
	struct sockaddr_in client;
	int c = sizeof(struct sockaddr_in);
	return accept(server_info->socket_descriptor, (struct sockaddr *)&client, (socklen_t*)&c);
}

int send_message(int fd, message_t *message, size_t message_size) {
	return write(client_fd, message, message_size);
}

int fetch_message(int fd, message_t *message, size_t message_size) {
    return recv(fd, message, message_size, 0);
}

int close_connection(int fd) {
	return close(fd);
}
