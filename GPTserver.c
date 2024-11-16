/*
server.c
CSC-2770 Program 3
Authors: Mitchell Koester & Justin Nelson
Last Modified: 11/16/2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_PACKET_SIZE 150
#define MAX_MESSAGE_SIZE 1024

int create_server_socket();
void bind_server_socket(int server_fd, struct sockaddr_in *address);
void handle_client(int server_fd);

int main() {
    int server_fd;
    struct sockaddr_in address;

    server_fd = create_server_socket();
    bind_server_socket(server_fd, &address);

    printf("Server is listening on port %d...\n", PORT);
    while(1)
    {
        //don't need accept as all packet transfer happens in handle_client
        handle_client(server_fd);
    }

    close(server_fd);
    return 0;
}

//Create Server Socket function.
//opens up a socket and returns the filed descriptor
//Parameters: none
//return: int serfver_fd
int create_server_socket() {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    //verify that socket creation was successful
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

//bind_server_socket function
//takes binds server file descriptor to a sockaddr
//parameters: int server_fd, socaddr_in* address
//returns void
void bind_server_socket(int server_fd, struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    //bind socket and verify there was no error
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
}

//handle_client functoin
//receives packets from client and reassembles them into a message
//parameters: int server_fd
//returns void
void handle_client(int server_fd) {

    //create buffer for receiving packet data and message for reassembly of total message
    char buffer[MAX_PACKET_SIZE];
    char message[MAX_MESSAGE_SIZE] = {0};
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    //keep received packets and total packets to verify we are receiving the correct number of packets
    int total_packets = 0;
    int received_packets = 0;

    // Receive total packet count
    recvfrom(server_fd, &total_packets, sizeof(total_packets), 0, (struct sockaddr *)&client_addr, &addr_len);

    //convert packet number into integer we can use
    total_packets = ntohl(total_packets);

    printf("Expecting %d packets...\n", total_packets);

    // Receive each packet and reassemble
    while (received_packets < total_packets) {
        int bytes_received = recvfrom(server_fd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Error receiving packet");
            continue;
        }

        //add terminating character so it is a valid cstring
        buffer[bytes_received] = '\0';

        //append the current buffer to the total message
        strcat(message, buffer); 
        received_packets++;

        // Send acknowledgment based on how many packets have been received
        sendto(server_fd, &received_packets, sizeof(received_packets), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    printf("Received message: %s\n", message);
}
