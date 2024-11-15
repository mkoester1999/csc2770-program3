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
    handle_client(server_fd);

    close(server_fd);
    return 0;
}

int create_server_socket() {
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void bind_server_socket(int server_fd, struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
}

void handle_client(int server_fd) {
    char buffer[MAX_PACKET_SIZE];
    char message[MAX_MESSAGE_SIZE] = {0};
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int total_packets, received_packets = 0;

    // Receive total packet count
    recvfrom(server_fd, &total_packets, sizeof(total_packets), 0, (struct sockaddr *)&client_addr, &addr_len);
    total_packets = ntohl(total_packets); // Convert to host byte order

    printf("Expecting %d packets...\n", total_packets);

    // Receive each packet and reassemble
    while (received_packets < total_packets) {
        int bytes_received = recvfrom(server_fd, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Error receiving packet");
            continue;
        }

        buffer[bytes_received] = '\0';
        strcat(message, buffer); // Append to message
        received_packets++;

        // Send acknowledgment
        sendto(server_fd, &received_packets, sizeof(received_packets), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    printf("Received message: %s\n", message);
}
