/*
client.c
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

int create_client_socket();
void configure_server_address(struct sockaddr_in *serv_addr);
void send_message(int client_socket, struct sockaddr_in *serv_addr, const char *message);

int main() {
    int client_socket;
    struct sockaddr_in serv_addr;

    //original message wasn't over 150 bytes so I added some characters at the end
    char *message = "This is an example of a very long message that will be broken into multiple 150-byte packets. aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    client_socket = create_client_socket();
    configure_server_address(&serv_addr);
    send_message(client_socket, &serv_addr, message);

    close(client_socket);
    return 0;
}

//create_client_socket function
//creates UDP socket
//parameters: none
//returns: socket file descriptor
int create_client_socket() {
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    //verify no errors in socket creation
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

//configure_server_address function
//takes in server address and configures for localhost communication over defined port
//parameters: sockaddr_in *
//returns: void
void configure_server_address(struct sockaddr_in *serv_addr) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr->sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
}

//send_message function
//sends a message to server address on given socket
//parameters: int client_socket, sockaddr_in * server_address, char* message
//returns void
void send_message(int client_socket, struct sockaddr_in *serv_addr, const char *message) {
    printf("%lu\n", (strlen(message) * sizeof(char)));

    //set total packets to the ceiling of size of message/MAX_PACKET_SIZE
    int total_packets = (strlen(message) + MAX_PACKET_SIZE -1) / MAX_PACKET_SIZE;
    int sent_packets = 0;
    char buffer[MAX_PACKET_SIZE];
    socklen_t addr_len = sizeof(*serv_addr);

    // convert int into network-byte order for transmission
    int total_packets_network = htonl(total_packets);
    
    //send total bytes to be transported to the server
    sendto(client_socket, &total_packets_network, sizeof(total_packets_network), 0, (struct sockaddr *)serv_addr, addr_len);

    while (sent_packets < total_packets) {
        int start_pos = sent_packets * MAX_PACKET_SIZE;

        //get the remaining length
        size_t remaining_length = strlen(message + start_pos);

        //determine bytes to send in the next packet 
        int bytes_to_send;
        if (remaining_length > MAX_PACKET_SIZE) 
        {
            bytes_to_send = MAX_PACKET_SIZE;
        }
        else 
        {
            bytes_to_send = remaining_length;
        }

        //add the current partition of the message to send to the buffer
        strncpy(buffer, message + start_pos, bytes_to_send);
        buffer[bytes_to_send] = '\0';

        // Send packet
        sendto(client_socket, buffer, bytes_to_send, 0, (struct sockaddr *)serv_addr, addr_len);

        // Wait for acknowledgment
        //make sure the acknowledgements includes the correct number of packets sent, otherwise we will loop through again and retransmit
        int ack;
        recvfrom(client_socket, &ack, sizeof(ack), 0, (struct sockaddr *)serv_addr, &addr_len);
        if (ack > sent_packets) {
            sent_packets++;
        }
    }

    printf("Message sent successfully in %d packets.\n", total_packets);
}
