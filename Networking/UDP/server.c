#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6767
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    char word_buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    FILE *fp;

    /*
        AF_INET: address family is IPv4
        SOCK_DGRAM: socket type is UDP
        0: lets OS choose protocoal (default is UDP)
     */
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Unable to create Socket\n");
        exit(EXIT_FAILURE);
    }

    // Configure Server Address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind Socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Server is running on port %d...\n", PORT); // `sudo lsof -i :6767` to verify if actually running or can do netstat

    // receive file
    int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
    buffer[n] = '\0';
    printf("Client request for file: %s\n", buffer);

    // check if file exists
    fp = fopen(buffer, "r");
    if (fp == NULL) {
        char temp_filename[BUFFER_SIZE];
        strcpy(temp_filename, buffer);
        sprintf(buffer, "NOT_FOUND %s", temp_filename);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
        printf("File not found. Exiting now\n");
        close(sockfd);
        exit(0);
    } else {
        char *msg = "FOUND";
        sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&client_addr, addr_len);
        printf("File found!\n");
    }

    // word transfer loop
    while(1){
        n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';

        // read from file
        if(fscanf(fp, "%s", word_buffer) !=  EOF){
            sendto(sockfd, word_buffer, strlen(word_buffer), 0, (struct sockaddr *)&client_addr, addr_len);
        }else{
            char *eof_msg = "EOF";
            sendto(sockfd, eof_msg, strlen(eof_msg), 0, (struct sockaddr *)&client_addr, addr_len);
            printf("yay! end of file reached!\n");
            break;
        }

    }

    fclose(fp);
    close(sockfd);
    return 0;
}
