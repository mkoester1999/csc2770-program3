// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_PACKET_SIZE 150  // Limit packet size to 150 bytes

// Function declarations
int create_client_socket();
void connect_to_server(int client_socket, struct sockaddr_in *serv_addr);
void send_message(int client_socket, const char *message);
void close_client_socket(int client_socket);

int main() {
    int client_socket;
    struct sockaddr_in serv_addr;
    char *message = "This is an example of a very long message that will be broken into multiple 150-byte packets.";

    client_socket = create_client_socket();
    connect_to_server(client_socket, &serv_addr);
    send_message(client_socket, message);
    close_client_socket(client_socket);

    return 0;
}

// Function to create the client socket
int create_client_socket() {
    // TODO: Implement client socket creation
    return 0;
}

// Function to connect the client to the server
void connect_to_server(int client_socket, struct sockaddr_in *serv_addr) {
    // TODO: Implement connecting the client to the server
    // You can emulate TCP behavior here. For now simply print which server you are connecting to.
    // More advanced clients will need to keep track of which server they are connecting to, you can ignore this for now.
}

// Function to send an arbitrarily long message to the server
void send_message(int client_socket, const char *message) {
    // TODO: Implement message sending in chunks of 150 bytes
}

// Function to close the client socket
void close_client_socket(int client_socket) {
    // TODO: Implement closing the client socket
}

