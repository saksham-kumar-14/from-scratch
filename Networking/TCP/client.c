#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6767
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 100
#define KEY_SIZE 26

int main() {
    char filename[256];
    char key[256];
    char cont_choice[10];

    while (1) {
        struct sockaddr_in serv_addr;
        int sockfd;

        while (1) {
            printf("Enter filename: ");
            scanf("%s", filename);
            if (access(filename, F_OK) == 0) break;
            printf("NOTFOUND %s\n", filename);
        }

        while (1) {
            printf("Enter key: ");
            scanf("%s", key);
            if (strlen(key) == KEY_SIZE) break;
            printf("Invalid key length.\n");
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            exit(1);
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
            perror("Invalid address");
            exit(1);
        }

        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            exit(1);
        }

        send(sockfd, key, KEY_SIZE, 0);

        FILE *fp = fopen(filename, "r");
        char buf[BUF_SIZE];
        int n;
        while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0) {
            send(sockfd, buf, n, 0);
        }
        fclose(fp);

        shutdown(sockfd, SHUT_WR);

        char out_filename[300];
        sprintf(out_filename, "%s.enc", filename);
        fp = fopen(out_filename, "w");

        while ((n = recv(sockfd, buf, BUF_SIZE, 0)) > 0) {
            fwrite(buf, 1, n, fp);
        }
        fclose(fp);
        close(sockfd);

        printf("The file is encrypted.\n");
        printf("Original file: %s\n", filename);
        printf("Encrypted file: %s\n", out_filename);

        printf("Do you want to encrypt another file? (Yes/No): ");
        scanf("%s", cont_choice);
        if (strcasecmp(cont_choice, "No") == 0) break;
    }

    return 0;
}
