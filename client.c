#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <error.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define MAX_DATA 4096

void main(int argc, char **argv)
{
    struct sockaddr_in remote_server; /* variable names for the socket addr data structure */
    int sock, len;
    char input[MAX_DATA];
    char output[MAX_DATA];

    if (argc != 3)
    {
        printf("Too few arguments \n");
        printf("Usage: %s <IP Addr> <Port Number> \n", argv[0]);
        exit(1);
    }

    /* socket creation */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(argv[2]));
    remote_server.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(&remote_server.sin_zero, 0);

    /* connecting */
    if (connect(sock, (struct sockaddr*)&remote_server, sizeof(remote_server)) == -1)
    {
        perror("connect");
        exit(1);
    }

    while (1)
    {
        fgets(input, MAX_DATA, stdin);
        send(sock, input, strlen(input), 0);
        len = recv(sock, output, MAX_DATA, 0);
        output[len] = '\0';
        printf("%s\n", output);
    }

    close(sock);
}
