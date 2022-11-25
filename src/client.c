#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    // user-defined server's ip and port
    /* Make sure both the client and server are having the same ip and port */
    char *ip = "127.0.0.1";
    int port = 3939;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];

    // create the client side socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-] Socket error!");
        exit(1);
    }

    printf("[+] TCP server socket created. \n");

    // define the server address
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to the server. \n");

    // send the message to the server socket
    char *msg = malloc(1024);
    printf("Enter a message: ");
    fgets(msg, 1024, stdin);
    bzero(buffer, 1024);
    strcpy(buffer, msg);
    free(msg); // de-allocate the memory
    printf("Client: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    // receive the message from the server socket
    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    // close the connection (socket)
    close(sock);
    printf("Disconnected from the server.\n");

    return 0;
}