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

void send_message(int client_socket, const char *message, size_t message_length, const struct sockaddr_in *server_addr) {
    if (message == NULL || server_addr == NULL) {
        fprintf(stderr, "Error: NULL message or server address\n");
        return;
    }

    socklen_t addr_len = sizeof(*server_addr);
    size_t total_packets = (message_length / MAX_PACKET_SIZE) + 
                           ((message_length % MAX_PACKET_SIZE) ? 1 : 0);

    printf("Sending a message of length %zu bytes in %zu packet(s)\n", message_length, total_packets);

    for (size_t i = 0; i < total_packets; ++i) {
        size_t packet_size = MAX_PACKET_SIZE;

        // Handle the last packet, which may be smaller than MAX_PACKET_SIZE
        if (i == total_packets - 1 && message_length % MAX_PACKET_SIZE != 0) {
            packet_size = message_length % MAX_PACKET_SIZE;
        }

        const char *packet_start = message + (i * MAX_PACKET_SIZE);

        // Send each packet and check for errors
        ssize_t sent_bytes = sendto(client_socket, 
                                    packet_start, 
                                    packet_size, 
                                    0, 
                                    (const struct sockaddr *)server_addr, 
                                    addr_len);

        if (sent_bytes == -1) {
            perror("sendto");
            fprintf(stderr, "Error sending packet %zu of %zu\n", i + 1, total_packets);
            return;
        } else if ((size_t)sent_bytes != packet_size) {
            fprintf(stderr, "Error: Partial send detected. Sent %zd bytes, expected %zu bytes\n", sent_bytes, packet_size);
            return;
        }

        printf("Packet %zu/%zu of size %zu bytes sent successfully\n", i + 1, total_packets, packet_size);
    }

    printf("Message sent successfully in %zu packet(s)\n", total_packets);
}


