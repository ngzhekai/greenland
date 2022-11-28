#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "../lib/menuoption.h"

/**
 * The client's flow of operations are done as depicted in following diagram:
 *  ┌──────────────┐       ┌────────────────┐
 *  │              │       │                │
 *  │ Display Menu │       │ Create Socket  │
 *  │              │       │                │
 *  └──────┬───────┘       └────────┬───────┘
 *         │                        │
 *         │                        │
 *  ┌──────▼───────┐       ┌────────▼───────┐
 *  │              │       │                │
 *  │ Select Menu  │       │Establish Socket│
 *  │              │       │                │
 *  └──────┬───────┘       └────────┬───────┘
 *         │                        │
 *         │                        │
 *         │               ┌────────▼───────┐
 *         │               │                │
 *         │               │   Send Menu    │
 *         └───────────────►    Option      │
 *                         │                │
 *                         └────────────────┘
 */
int main(void)
{
  pid_t child_pid;
  MenuOption option;

  if ((child_pid = fork()) == -1) {
    perror("[-] Fork error!");
    exit(3);
  } else if (child_pid == 0) {
    moption_display(&option);

    if (moption_handle(option) == -1) {
      printf("Option %d not supported\n", option);
      exit(4);
    }
  } else {

    // user-defined server's ip and port
    /* Make sure both the client and server are having the same ip and port */
    char ip[] = "127.0.0.1";
    int port = 3939;

    int sock;
    struct sockaddr_in addr;
    // socklen_t addr_size;
    char buffer[1024];

    // create the client side socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
      perror("[-] Socket error!");
      exit(1);
    }

    printf("[+] TCP server socket created. \n");

    // define the server address
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
      perror("[-] Connection error!");
      exit(2);
    }

    printf("Connected to the server. \n");
    wait((int*) 0);

    // send the message to the server socket
    bzero(buffer, 1024);
    snprintf(buffer, 1024, "%d", option);
    printf("Client: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    // receive the message from the server socket
    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    // close the connection (socket)
    close(sock);
    printf("Disconnected from the server.\n");
  }

  return 0;
}
