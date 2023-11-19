#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUFSIZE 30

void error_handling(const char *message);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error_handling("WSAStartup() error");
    }

    SOCKET sock;
    struct sockaddr_in serv_addr;
    char message[BUFSIZE];
    int str_len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        error_handling("connect() error");
    } else {
        puts("Connected...........");
    }

    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUFSIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }
        printf("trying to send %s to server", message);
        if (send(sock, message, strlen(message), 0) == -1)
            error_handling("send() error");
        str_len = recv(sock, message, BUFSIZE - 1, 0);
        if (str_len == -1) {
            error_handling("read() error");
        }
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
