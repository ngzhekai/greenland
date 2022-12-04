#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../lib/menuoption.h"

int main(void)
{
  // user-defined server's ip and port
  /* Make sure both the client and server are having the same ip and port */
  char ip[] = "127.0.0.1";
  int port = 3939;

  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;
  char buffer[1024];

  // create the server side socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket < 0) {
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
  int bind_status = bind(server_socket, (struct sockaddr*)&server_addr,
                         sizeof(server_addr));

  if (bind_status < 0) {
    perror("[-] Bind error!");
    exit(2);
  }

  printf("[+] Bind to the port number: %d\n", port);

  // 5 connection request that can be queue by the system in server_socket
  listen(server_socket, 5);
  printf("Listening...\n");

  while (1) {
    // infinite loop inside the while(), which will not end the server
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr,
                           &addr_size);
    printf("[+] Client connected.\n");

    // receive the message from the client socket
    bzero(buffer, 1024);
    recv(client_socket, buffer, sizeof(buffer), 0);
    MenuOption client_menuoption = strtol(buffer, NULL, 10);
    printf("Client: Request Option %d\n", client_menuoption);

    // handling menu option
    bzero(buffer, 1024);
    snprintf(buffer, 1024, "%s", moption_handle(client_menuoption));

    // send the message to the client socket
    printf("Server: %s\n", buffer);
    send(client_socket, buffer, strlen(buffer), 0);

    // close the connection with the client (socket)
    close(client_socket);
    printf("[+] Client disconnected. \n\n");
  }

  return 0;
}
