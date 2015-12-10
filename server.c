#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define PORT 3013

void quit();
int setup(int);
void *recvSecretFile();

char *ip = "127.0.0.1";
int server_d = 0;
int client_d = 0;

int main(int argc, char *argv[])
{
    if (argc == 2) ip = argv[1];
    printf("Running at %s\n", ip);

    signal(SIGINT, quit);

    pthread_t thread;
    server_d = setup(PORT);

    if (pthread_create(&thread, NULL, recvSecretFile, NULL))
        errx(1, "Error creating thread");

    pthread_join(thread, NULL);

    unlink(ip);
    close(server_d);

    return 0;
}

void quit()
{
    printf("\nBye\n");

    if (server_d) close(server_d);
    if (client_d) close(client_d);
    exit(0);
}

int setup(int port)
{
    int socket_id;
    struct sockaddr_in addr_server;

    // socket()
    if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errx(1, "Error creating socket");

    // bind()
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);
    addr_server.sin_addr.s_addr = inet_addr(ip);
    bzero(&(addr_server.sin_zero), 8);

    if (bind(socket_id, (struct sockaddr *) &addr_server, sizeof(struct sockaddr)) == -1)
    {
        close(socket_id);
        errx(1, "Error executing bind()");
    }

    // listen()
    if (listen(socket_id, 10) < 0)
    {
        close(socket_id);
        errx(1, "Error executing listen()");
    }

    return socket_id;
}

void *recvSecretFile()
{
    FILE *recvFile = fopen("secretfile.y", "a+");

    while (1)
    {
        struct sockaddr addr_client;
        socklen_t client_len;

        client_d = accept(server_d, (struct sockaddr *) &addr_client, &client_len);
        if (client_d < 0)
            continue;

        int length;
        char *buffer;

        if (recv(client_d, &length, sizeof(length), 0) == 0)
            continue;

        buffer = (char *) malloc(length);
        if (recv(client_d, buffer, length, 0) == 0)
            continue;

        fwrite(buffer, sizeof(char), length, recvFile);

        char *msg = "OK";
        int len = strlen(msg) + 1;
        if (send(client_d, &len, sizeof(len), 0) == -1)
        {
            close(client_d);
            errx(1, "Error sending message to server");
        }

        if (send(client_d, msg, len, 0) == -1)
        {
            close(client_d);
            errx(1, "Error sending message");
        }

        printf("OK\n");

        free(buffer);
        close(client_d);
    }

    fclose(recvFile);
}
