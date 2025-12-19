#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 5
#define DEFAULT_PORT 6000
#define BUFFER_SIZE 1024

static int create_server_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("[serveur] Listening on port %d\n", port);
    int server_fd = create_server_socket(port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[BUFFER_SIZE];
        
        // Message 1: Receive "Hi" from client
        ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            printf("[serveur] Received: %s\n", buffer);
            
            const char *response1 = "Hello! How are you?";
            send(client_fd, response1, strlen(response1), 0);
            printf("[serveur] Sent: %s\n", response1);
        }
        
        // Message 2: Receive "I'm fine" from client
        received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            printf("[serveur] Received: %s\n", buffer);
            
            const char *response2 = "Great! Nice to talk with you!";
            send(client_fd, response2, strlen(response2), 0);
            printf("[serveur] Sent: %s\n", response2);
        }
        
        // Message 3: Receive "Goodbye" from client
        received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            printf("[serveur] Received: %s\n", buffer);
            
            const char *response3 = "Goodbye! Have a great day!";
            send(client_fd, response3, strlen(response3), 0);
            printf("[serveur] Sent: %s\n", response3);
        }
        
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
