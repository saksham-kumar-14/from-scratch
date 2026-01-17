#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 6767
#define BUF_SIZE 100
#define KEY_SIZE 26

void sigchld_handler(int s) {
    (void)s;
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void encrypt_file(const char *input_path, const char *output_path, const char *key) {
    FILE *fin = fopen(input_path, "r");
    FILE *fout = fopen(output_path, "w");
    if (!fin || !fout) return;

    char ch;
    while ((ch = fgetc(fin)) != EOF) {
        if (isalpha(ch)) {
            if (isupper(ch)) {
                fputc(key[ch - 'A'], fout);
            } else {
                fputc(tolower(key[ch - 'a']), fout);
            }
        } else {
            fputc(ch, fout);
        }
    }
    fclose(fin);
    fclose(fout);
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int pid;
    struct sigaction sa;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, 5);

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            continue;
        }

        pid = fork();
        if (pid < 0) {
            perror("ERROR on fork");
            exit(1);
        }

        if (pid == 0) {
            close(sockfd);

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(cli_addr.sin_port);

            char fname[256];
            sprintf(fname, "%s.%d.txt", client_ip, client_port);

            char key[KEY_SIZE + 1];
            int total_read = 0;
            while (total_read < KEY_SIZE) {
                int n = recv(newsockfd, key + total_read, KEY_SIZE - total_read, 0);
                if (n <= 0) {
                    close(newsockfd);
                    exit(1);
                }
                total_read += n;
            }
            key[KEY_SIZE] = '\0';

            FILE *fp = fopen(fname, "w");
            if (!fp) {
                close(newsockfd);
                exit(1);
            }

            char buf[BUF_SIZE];
            int n;
            while ((n = recv(newsockfd, buf, BUF_SIZE, 0)) > 0) {
                fwrite(buf, 1, n, fp);
            }
            fclose(fp);

            char enc_fname[260];
            sprintf(enc_fname, "%s.enc", fname);
            encrypt_file(fname, enc_fname, key);

            fp = fopen(enc_fname, "r");
            if (fp) {
                while ((n = fread(buf, 1, BUF_SIZE, fp)) > 0) {
                    send(newsockfd, buf, n, 0);
                }
                fclose(fp);
            }

            close(newsockfd);
            exit(0);
        } else {
            close(newsockfd);
        }
    }
    return 0;
}
