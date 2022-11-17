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

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];

    // create the server side socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("[-] Socket error!");
        exit(1);
    }

    printf("[+] TCP server socket created. \n");

    // define the server address
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    // inet_addr() convert numbers and dots (readable IP) notation into binary format

    // bind the server socket to the ip and port
    int bind_status = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_status < 0)
    {
        perror("[-] Bind error!");
        exit(1);
    }

    printf("[+] Bind to the port number: %d\n", port);

    // 5 connection request that can be queue by the system in server_socket
    listen(server_socket, 5);
    printf("Listening...\n");

    // infinite loop inside the while(), which will not end the server
    while (1)
    {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        printf("[+] Client connected.\n");

        // receive the message from the client socket
        bzero(buffer, 1024);
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Client: %s\n", buffer);

        // send the message to the client socket
        char *msg = malloc(1024);
        printf("Enter a message: ");
        fgets(msg, 1024, stdin);
        bzero(buffer, 1024);
        strcpy(buffer, msg);
        free(msg); // de-allocate the memory
        printf("Server:%s\n", buffer);
        send(client_socket, buffer, strlen(buffer), 0);

        // close the connection with the client (socket)
        close(client_socket);
        printf("[+] Client disconnected. \n\n");
    }

    return 0;
}