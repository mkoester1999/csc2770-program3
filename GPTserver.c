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
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    size_t total_received = 0;
    char *reconstructed_message = (char *)malloc(1); // Start with a small buffer

    printf("Ready to receive data...\n");

    while (1) {
        ssize_t bytes_received = recvfrom(server_fd, buffer, MAX_PACKET_SIZE, 0, 
                                          (struct sockaddr *)&client_addr, &addr_len);

        if (bytes_received == -1) {
            perror("recvfrom");
            break;
        }

        // Resize the reconstructed message buffer
        reconstructed_message = (char *)realloc(reconstructed_message, total_received + bytes_received);
        memcpy(reconstructed_message + total_received, buffer, bytes_received);
        total_received += bytes_received;

        printf("Received packet of size %ld bytes. Total received: %ld bytes.\n", bytes_received, total_received);

        // Check for termination (optional protocol logic)
        if (bytes_received < MAX_PACKET_SIZE) {
            printf("Last packet received.\n");
            break;
        }
    }

    // Null-terminate and print the reconstructed message for safety
    reconstructed_message = (char *)realloc(reconstructed_message, total_received + 1);
    reconstructed_message[total_received] = '\0';
    printf("Reconstructed message: %s\n", reconstructed_message);

    free(reconstructed_message);
}

