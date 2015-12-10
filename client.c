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
    fseek(fp, 0, SEEK_SET);

    char *buffer = (char *) malloc(fsize + 1);
    fread(buffer, fsize, sizeof(char), fp);
    fclose(fp);

    FILE *test = fopen("test.y", "w");
    fprintf(test, "%s\n", buffer);

    socket_d = setup(ip, PORT);
    int length = strlen(buffer) + 1;
    if (write(socket_d, &length, sizeof(length)) == -1)
    {
        close(socket_d);
        errx(1, "Error sending message to server");
    }

    if (write(socket_d, buffer, length) == -1)
    {
        close(socket_d);
        errx(1, "Error sending message to server");
    }

    char *msg;
    if (recv(socket_d, &length, sizeof(length), 0) == -1)    
    {
        close(socket_d);
        errx(1, "Error sending message to server");
    }

    msg = (char *) malloc(length);
    if (recv(socket_d, msg, length, 0) == -1)
    {
        close(socket_d);
        errx(1, "Error sending message to server");
    }

    if (strcmp(msg, "OK") == 0)
        printf("Sent to server successfully!\n");
    else
        printf("An error has occurred!\n");

    if (socket_d) close(socket_d);
    free(buffer);
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
