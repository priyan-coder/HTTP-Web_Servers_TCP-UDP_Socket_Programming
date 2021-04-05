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
// #include <signal.h>

#define MAX_DATA 4096

// static volatile int keepRunning = 1;

// void intHandler(int dummy)
// {
//     keepRunning = 0;
// }

void main()
{
    pid_t pid;                     /* process id */
    int listenfd, connfd;          /* socket descriptor */
    struct sockaddr_in s1, client; /* variable names for the socket addr data structure */
    int cli_len = sizeof(client);
    int total, sent, bytes;            /* varibles related to sending reply */
    int total_size, received, n_bytes; /* variables related to receiving response */
    char buf[MAX_DATA];                /* data sent by client stored in buf */
    char *reply =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>\r\n"
        "\n"
        "<html><head><title>EE4210_CA2_PRIYAN</title></head>\r\n"
        "<body><form><input type = \"text\"><input type = \"submit\"</form></body><html>\r\n";

    // signal(SIGINT, intHandler);

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

            /* sending the HTML file*/
            total = strlen(reply);
            sent = 0;
            do
            {
                int bytes = write(connfd, reply + sent, total - sent);
                if (bytes < 0)
                    perror("ERROR writing message to socket");
                if (bytes == 0)
                    break;
                sent += bytes;
            } while (sent < total);

            memset(buf, 0, sizeof(buf));
            total = sizeof(buf) - 1;
            received = 0;
            do
            {
                bytes = read(connfd, buf + received, total - received);
                if (bytes < 0)
                    perror("ERROR reading response from socket");
                if (bytes == 0)
                    break;
                received += bytes;
            } while (received < total);

            if (received == total)
                perror("ERROR storing complete response from socket");

            printf("client disconnected\n");
            buf[n_bytes] = '\0';
            printf("Printing BUF : %s\n", buf);
            printf("Goodbye\n");
        }
    }

    /* parent */
    close(connfd);
    exit(0);
}
