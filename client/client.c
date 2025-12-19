#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 6000
#define BUFFER_SIZE 1024

static int connect_to_server(const char *host, int port) {
    struct addrinfo hints;
    struct addrinfo *result;
    char port_str[16];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%d", port);

    int rc = getaddrinfo(host, port_str, &hints, &result);
    if (rc != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
        exit(EXIT_FAILURE);
    }

    int sockfd = -1;
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; // success
        }
        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);

    if (sockfd == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int main(int argc, char *argv[]) {
    const char *host = DEFAULT_HOST;
    int port = DEFAULT_PORT;

    if (argc > 1) host = argv[1];
    if (argc > 2) port = atoi(argv[2]);

    printf("[client] Connecting to %s:%d\n", host, port);
    int sockfd = connect_to_server(host, port);

    char buffer[BUFFER_SIZE];
    
    // Message 1: Say "Hi" to server
    const char *message1 = "Hi, Server!";
    printf("[client] Sending: %s\n", message1);
    if (send(sockfd, message1, strlen(message1), 0) < 0) {
        perror("send");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    // Receive response
    ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        printf("[client] Received: %s\n", buffer);
    }
    
    // Message 2: Respond "I'm fine"
    const char *message2 = "I'm fine, thank you!";
    printf("[client] Sending: %s\n", message2);
    send(sockfd, message2, strlen(message2), 0);
    
    // Receive response
    received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        printf("[client] Received: %s\n", buffer);
    }
    
    // Message 3: Say "Goodbye"
    const char *message3 = "Goodbye, Server!";
    printf("[client] Sending: %s\n", message3);
    send(sockfd, message3, strlen(message3), 0);
    
    // Receive final response
    received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (received > 0) {
        buffer[received] = '\0';
        printf("[client] Received: %s\n", buffer);
    } else if (received == 0) {
        printf("[client] Connection closed by server\n");
    } else {
        perror("recv");
    }

    close(sockfd);
    return 0;
}
