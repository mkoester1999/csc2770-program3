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
    char *message = "This is an example of a very long message that will be broken into multiple 150-byte packets.";

    client_socket = create_client_socket();
    configure_server_address(&serv_addr);
    send_message(client_socket, &serv_addr, message);

    close(client_socket);
    return 0;
}

int create_client_socket() {
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

void configure_server_address(struct sockaddr_in *serv_addr) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr->sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
}

void send_message(int client_socket, struct sockaddr_in *serv_addr, const char *message) {
    int total_packets = (strlen(message) + MAX_PACKET_SIZE - 1) / MAX_PACKET_SIZE;
    int sent_packets = 0;
    char buffer[MAX_PACKET_SIZE];
    socklen_t addr_len = sizeof(*serv_addr);

    // Send total packet count
    int total_packets_network = htonl(total_packets);
    sendto(client_socket, &total_packets_network, sizeof(total_packets_network), 0, (struct sockaddr *)serv_addr, addr_len);

    while (sent_packets < total_packets) {
        int start_idx = sent_packets * MAX_PACKET_SIZE;
        int bytes_to_send = strlen(message + start_idx) > MAX_PACKET_SIZE ? MAX_PACKET_SIZE : strlen(message + start_idx);

        strncpy(buffer, message + start_idx, bytes_to_send);
        buffer[bytes_to_send] = '\0';

        // Send packet
        sendto(client_socket, buffer, bytes_to_send, 0, (struct sockaddr *)serv_addr, addr_len);

        // Wait for acknowledgment
        int ack;
        recvfrom(client_socket, &ack, sizeof(ack), 0, (struct sockaddr *)serv_addr, &addr_len);
        if (ack > sent_packets) {
            sent_packets++;
        }
    }

    printf("Message sent successfully in %d packets.\n", total_packets);
}
