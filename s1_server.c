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
#define SERVER_PORT 1200

// TCP Concurrent Server using fork
void main()
{
    pid_t pid;                     /* process id */
    int listenfd, connfd;          /* socket descriptor */
    struct sockaddr_in s1, client; /* variable names for the socket addr data structure */
    int cli_len = sizeof(client);
    int sent, received;
    char buf[MAX_DATA]; /* current data sent by client stored in buf of server */
    char headers[MAX_DATA] = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                             "<!DOCTYPE html>\r\n"
                             "\n"
                             "<html><head><title>EE4210_CA2_PRIYAN</title></head>\r\n";
    char *reply = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  "<!DOCTYPE html>\r\n"
                  "\n"
                  "<html><head><title>EE4210_CA2_PRIYAN</title></head>\r\n"
                  "<body><form method = \"post\" action = \"/\"><p>Please type your input here !</p><input name = \"userin\" type = \"text\"></form></body><html>\r\n";

    /* socket creation */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket\n");
        exit(1);
    }

    s1.sin_family = AF_INET;
    s1.sin_port = htons(SERVER_PORT);
    s1.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&s1.sin_zero, 0);

    /* binding */
    if (bind(listenfd, (struct sockaddr *)&s1, sizeof(s1)) < 0)
    {
        perror("bind\n");
        exit(1);
    }

    /* listening */
    if (listen(listenfd, 10) < 0)
    {
        perror("listen\n");
        exit(1);
    }

    /* accepting with implementation of concurrent server using fork() */
    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)&client, &cli_len);
        if (connfd < 0)
        {
            perror("accept\n");
            exit(1);
        }

        printf("New Client connected from port number %d and IP address  %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));

        /* child */
        if ((pid = fork()) == 0)
        {
            close(listenfd);

            /* receiving request */
            memset(buf, 0, sizeof(buf));
            received = 0;

            received = recv(connfd, buf, MAX_DATA - 1, 0);
            if (received <= 0)
            {
                perror("Didnt read any from client\n");
                exit(1);
            }

            buf[received] = '\0';
            printf("Currently stored in buffer :\n %s\n", buf);
            printf("Received %d bytes in child\n\n", received);

            /* Request Handlers */

            /* Handle GET req*/
            if (strstr(buf, "GET") != NULL && strstr(buf, "HTTP/1.1") != NULL)
            {
                printf("Detected GET\n\n");

                /* sending the HTML file*/
                sent = 0;
                sent = send(connfd, reply, strlen(reply), 0);
                if (sent <= 0)
                {
                    perror("Didnt send any from server\n");
                    exit(1);
                }
                printf("Handled GET req and sent : \n %s", reply);
            }
            /* Handle POST req*/

            if (strstr(buf, "POST") != NULL && strstr(buf, "HTTP/1.1") != NULL)
            {
                printf("Detected POST\n\n");
                char *last = strrchr(buf, '=');
                if (last != NULL)
                {
                    printf("User entered Input: '%s'\n", last + 1);
                }

                char *head_new_body = "<body><p>";
                char *end_new_body = "</p></body><html>\r\n";
                char *text = "You entered the following : ";
                strcat(headers, head_new_body);
                strcat(headers, text);
                strcat(headers, last + 1);
                strcat(headers, end_new_body);
                sent = 0;
                sent = send(connfd, headers, strlen(headers), 0);
                if (sent <= 0)
                {
                    perror("Didnt send any from server\n");
                    exit(1);
                }
                printf("Handled POST req and sent back the following : \n %s", headers);
            }

            printf("Closing Child Process\n\n");
            close(connfd);
            exit(0);
        }
    }

    /* parent */
    close(connfd);
    printf("Closing Parent process\n\n");
    exit(0);
}
