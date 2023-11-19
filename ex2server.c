#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <winsock2.h> // windows에서 실행하기 위해서 필요

#define BUFSIZE 30

//gcc -o server ex2server.c -lwsock32

void error_handling(char *message);

int main(int argc, char **argv)
{
    // windows에서 소켓 함수를 사용하려면 wsastartup()과 wsacleanup()을 호출해서 라이브러리 초기화를 해줘야함.
    WSADATA wsaData; 

    int serv_sock;
    struct sockaddr_in serv_addr;
    fd_set reads, temps;
    int fd_max;
    char message[BUFSIZE];
    int str_len;
    struct timeval timeout;
    
    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        error_handling("WSAStartup() error");
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)))
        error_handling("bind() error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen() error");
    
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while(1){
        int fd=0, str_len=0;
        int clnt_sock, clnt_len;
        struct sockaddr_in clnt_addr;

        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        if (select(fd_max+1, &temps, 0, 0, &timeout) == -1)
            error_handling("select() error");
        for (fd= 0; fd< fd_max+1; fd++){
            if (FD_ISSET(fd, &temps)){
                if (fd== serv_sock) { 
                    clnt_len = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_len);
                    FD_SET(clnt_sock, &reads);
    
                    if (fd_max < clnt_sock)
                        fd_max=clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                } else {
                    printf("%d is trying to read()", fd);
                    str_len = read(fd, message, BUFSIZE);
                    if(str_len == -1) error_handling("read() error");
                    if(str_len == 0) {
                        FD_CLR(fd, &reads);
                        close(fd);
                        printf("closed client: %d \n", fd);
                    } else {
                        int check_write = write (fd, message, str_len);
                        if (check_write == -1)
                            error_handling("write() error");
                    }
                }
            }
        }
        printf("for loop is finished once.\n");
    }
    WSACleanup();
}

void error_handling(char *message) {
    perror("read error");
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}