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

#define MAX_DATA 2048

void main()
{
    pid_t pid;                     /* process id */
    int listenfd, connfd;          /* socket descriptor */
    struct sockaddr_in s1, client; /* variable names for the socket addr data structure */
    int cli_len = sizeof(client);
    int data_len; /* length of data sent by client */
    char buf[MAX_DATA];

    /* socket creation */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    s1.sin_family = AF_INET;
    s1.sin_port = htons(1200); /* using port 1200 for s1 */
    s1.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&s1.sin_zero, 0);

    /* binding */
    if (bind(listenfd, (struct sockaddr *)&s1, sizeof(s1)) < 0)
    {
        perror("bind");
        exit(1);
    }

    /* listening */
    if (listen(listenfd, 10) < 0)
    {
        perror("listen");
        exit(1);
    }

    /* accepting with implementation of concurrent server using fork() */
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&client, &cli_len);
        if (connfd < 0)
        {
            perror("accept");
            exit(1);
        }

        printf("New Client connected from port number %d and IP address  %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));

        /* child */
        if ((pid = fork()) == 0)
        {
            close(listenfd);
            /* processing */
            data_len = 1;
            while (data_len)
            {
                data_len = recv(connfd, buf, MAX_DATA, 0);
                if (data_len > 0)
                {
                    FILE *html_data;
                    html_data = fopen("index.html", "r");
                    char response_data[1024];
                    int size = fread(response_data, (sizeof(response_data) - 1), 1, html_data);
                    response_data[size] = '\0'; // Add null terminator to string
                    char http_header[MAX_DATA] = "HTTP/1.1 200 OK\r\n\n";
                    strcat(http_header, response_data);
                    send(connfd, http_header, sizeof(http_header), 0);
                    printf("sent html to client\n");
                }
            }
            printf("client disconnected\n");
            close(connfd);
            exit(0);
        }

        /* parent */
        close(connfd);
    }
}