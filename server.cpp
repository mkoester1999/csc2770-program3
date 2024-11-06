// server.c
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_PACKET_SIZE 150  // Limit packet size to 150 bytes
#define BACKLOG 10
// Function declarations
int create_server_socket();
void bind_server_socket(int server_fd, struct sockaddr_in *address);
int accept_client_connection(int server_fd, struct sockaddr_in *address);
void handle_client(int client_socket);
void close_server_socket(int server_fd);
void listen_for_connections(int server_fd);

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    
    server_fd = create_server_socket();
    bind_server_socket(server_fd, &address);
    
    //don't need to listen in UDP
    //listen_for_connections(server_fd);

    while (1) {
        client_socket = accept_client_connection(server_fd, &address);
        handle_client(client_socket);
    }

    close_server_socket(server_fd);
    return 0;
}

void listen_for_connections(int server_fd) {
    //don't use listen in UDP
    listen(server_fd, BACKLOG);
}

// Function to create the server socket
int create_server_socket() {
    // TODO: Implement server socket creation
    int server_fd = socket(AF_INET,SOCK_DGRAM, 0);

    return server_fd;
}

// Function to bind the server socket to an address and port
void bind_server_socket(int server_fd, struct sockaddr_in *address) {
    // TODO: Implement binding the server socket to an address

    int bind_result = bind(server_fd, (struct sockaddr *)address, sizeof(*address));

    return;
}

// Function to accept client connections
int accept_client_connection(int server_fd, struct sockaddr_in *address) {
    // TODO: Implement accepting client connection
    // You this is where you keep state related to the client. This might be useful for retransmission.
    socklen_t address_len = sizeof(*address);
    return accept(server_fd, (struct sockaddr *)address, (socklen_t*) &address_len);
}

// Function to handle communication with the client
void handle_client(int client_socket) {
    // TODO: Implement the logic to receive and send data to the client
    
    //receive data

}

// Function to close the server socket
void close_server_socket(int server_fd) {
    // TODO: Implement closing the server socket
    close(server_fd);
}

