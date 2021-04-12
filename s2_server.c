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

#define MAX_DATA 1024
#define SERVER_PORT 1600
// run in cmd line : gcc s2_server.c - lm - o s2
// ./s2

void main()
{
    pid_t pid;                     /* process id */
    int fd;                        /* socket descriptor */
    struct sockaddr_in s2, client; /* variable names for the socket addr data structure */
    int cli_len = sizeof(client);
    int sent, received;
    char buf[MAX_DATA]; /* current data sent by client stored in buf of server */
    char *reply = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  "<!DOCTYPE html>\r\n"
                  "\n"
                  "<html><head><title>EE4210_CA2_PRIYAN</title></head>\r\n"
                  "<body><p>EE-4210 Continuous assesment</p></body><html>\r\n";

    /* socket creation */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket\n");
        exit(1);
    }

    s2.sin_family = AF_INET;
    s2.sin_port = htons(SERVER_PORT);
    s2.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&s2.sin_zero, 0);

    /* binding */
    if (bind(fd, (struct sockaddr *)&s2, sizeof(s2)) < 0)
    {
        perror("bind\n");
        exit(1);
    }
    while (1)
    {
        /* receiving request */
        memset(buf, 0, sizeof(buf));
        received = 0;

        received = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &cli_len);
        if (received <= 0)
        {
            perror("Didnt read any from client\n");
            exit(1);
        }

        printf("New Client connected from port number %d and IP address  %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));

        buf[received] = '\0';
        printf("Currently stored in buffer :\n %s\n", buf);
        printf("Received %d bytes in child\n\n", received);

        /* child */
        if ((pid = fork()) == 0)
        {

            /* implementation of concurrent server using fork() */

            /* Handle GET req*/
            if (strstr(buf, "GET") != NULL)
            {
                printf("Detected GET\n\n");

                /* sending the HTML file*/
                sent = 0;
                sent = sendto(fd, reply, strlen(reply), 0, (struct sockaddr *)&s2, sizeof(s2));
                if (sent <= 0)
                {
                    perror("Didnt send any from server\n");
                    exit(1);
                }
                printf("Handled GET req and sent : \n %s", reply);
            }

            printf("Exiting Child Process\n\n");
            exit(0);
        }
        // printf("Looping to create a new child for handling request \n");
    }
    printf("Closing Socket in Parent\n");
    close(fd);
}
