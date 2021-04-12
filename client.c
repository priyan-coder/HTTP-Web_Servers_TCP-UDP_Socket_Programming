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
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdarg.h>

#define MAX_DATA 4096

void main(int argc, char **argv)
{
    struct sockaddr_in remote_server; /* variable names for the socket addr data structure */
    int sock, len, sent;
    char input[MAX_DATA];
    char *request = "GET HTTP/1.1\r\n";

    if (argc != 3)
    {
        printf("Too few arguments \n");
        printf("Usage: %s <IP Addr> <Port Number> \n", argv[0]);
        exit(1);
    }

    /* socket creation */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(argv[2]));
    remote_server.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(&remote_server.sin_zero, 0);

    sent = sendto(sock, request, strlen(request), 0, (struct sockaddr *)&remote_server, sizeof(remote_server));
    printf("Sent\n");
    len = recvfrom(sock, input, sizeof(input), 0, (struct sockaddr *)&remote_server, (socklen_t *)sizeof(remote_server));
    printf("Received\n ");
    input[len] = '\0';
    printf("Received from, server : %s\n", input);
    close(sock);
    exit(0);
}
