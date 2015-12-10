#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 3013

int sendToServer(FILE*, char*);
int setup(char*, int);

int socket_d = 0;

int sendToServer(FILE *fp, char *ip = "127.0.0.1")
{
    printf("Server at: %s\n", ip);

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);


    int length = 4096;
    char buffer[4096];
    int bytes = 0;
    while ((bytes = fread(buffer, sizeof(char), length, fp)))
    {
        socket_d = setup(ip, PORT);
        if (send(socket_d, &bytes, sizeof(bytes), 0) == -1)
        {
            close(socket_d);
            printf("1 -");
            errx(1, "Error sending message to server");
        }

        if (send(socket_d, buffer, bytes, 0) == -1)
        {
            close(socket_d);
            printf("2 -");
            errx(1, "Error sending message to server");
        }

        char *msg;
        int len;
        if (recv(socket_d, &len, sizeof(len), 0) == -1)
        {
            close(socket_d);
            errx(1, "Error receiving message");
        }

        msg = (char *) malloc(len);
        if (recv(socket_d, msg, len, 0) == -1)
        {
            close(socket_d);
            errx(1, "Error receiving message");
        }

        if (strcmp("OK", msg) == 0) printf("%s\n", msg);
    }

    if (socket_d) close(socket_d);
}

int setup(char *ip, int port)
{
    int socket_id;
    struct sockaddr_in addr_client;

    // socket()
    if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errx(1, "Error creating socket");

    // connect()
    addr_client.sin_family = AF_INET;
    addr_client.sin_port = htons(port);
    addr_client.sin_addr.s_addr = inet_addr(ip);
    bzero(&(addr_client.sin_zero), 8);
    if (connect(socket_id, (struct sockaddr *) &addr_client, sizeof(struct sockaddr)) == -1)
    {
        close(socket_id);
        errx(1, "Error connecting server");
    }

    return socket_id;
}
