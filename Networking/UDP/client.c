#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6767
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    // Init UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    printf("Enter filename to request: ");
    scanf("%s", filename);

    // Send filename
    sendto(sockfd, filename, strlen(filename), 0, (const struct sockaddr *)&server_addr, addr_len);

    // Receive initial response
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (n < 0) {
        perror("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';

    if (strncmp(buffer, "NOT_FOUND", 9) == 0) {
        printf("Server says: %s\n", buffer);
        close(sockfd);
        exit(0);
    }

    printf("Yup file exists, downloading now..\n");

    // Create local file
    FILE *fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("Cannot create file\n");
        exit(EXIT_FAILURE);
    }

    // Request word loop
    int word_count = 1;
    while (1) {
        char req[50];
        sprintf(req, "WORD_%d", word_count);

        // Send Request
        sendto(sockfd, req, strlen(req), 0, (const struct sockaddr *)&server_addr, addr_len);

        // Receive Word
        n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
        buffer[n] = '\0';

        if (strcmp(buffer, "EOF") == 0) {
            printf("\nTransfer Complete\n");
            break;
        }

        fprintf(fp, "%s ", buffer);
        printf("Received: %s\n", buffer);
        ++word_count;
    }

    fclose(fp);
    close(sockfd);
    return 0;
}
