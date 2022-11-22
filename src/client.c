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

    printf("  ________                               .__                       .___ \n");
    printf(" /  _____/_______   ____   ____    ____  |  |  _____     ____    __| _/ \n");
    printf("/   \\  ___\\_  __ \\_/ __ \\_/ __ \\  /    \\ |  |  \\__  \\   /    \\  / __ |  \n");
    printf("\\    \\_\\  \\|  | \\/\\  ___/\\  ___/ |   |  \\|  |__ / __ \\_|   |  \\/ /_/ |  \n");
    printf(" \\______  /|__|    \\___  >\\___  >|___|  /|____/(____  /|___|  /\\____ |  \n");
    printf("        \\/             \\/     \\/      \\/            \\/      \\/      \\/  \n\n");

    printf("Welcome to greenland, a place to plant tree!\n\n");
    printf("We don't have any affiliation to the Greenland government, but if \n"
           "you can plant a tree there, we are more than welcome! :)\n");

    printf("################################\n");
    printf("#                              #\n");
    printf("#            MENU              #\n");
    printf("#                              #\n");
    printf("################################\n\n");
    printf("\t1. To plant a tree\n");
    printf("\t2. To query a tree\n");
    printf("\t3. To update a tree's status\n");
    printf("(Choose a service needed according to the number)\n\n");
    printf("Option: ");

    scanf("%d", (int*) &option);
    system("clear");

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
    char* msg = malloc(1024);
    snprintf(msg, 1024, "%d", option);
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
  }

  return 0;
}
