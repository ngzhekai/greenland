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
#include "../lib/plant_tree.h"

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
  int pipe1[2];

  if (pipe(pipe1) == -1) {
    perror("[-] Pipe error!");
    exit(1);
  }

  if ((child_pid = fork()) == -1) {
    perror("[-] Fork error!");
    exit(2);
  } else if (child_pid == 0) {
    close(pipe1[0]);  // close read end of the pipe

    while (true) {
      char buffer[1024];
      MenuOption option;
      moption_display(&option);

      bzero(buffer, 1024);
      sprintf(buffer, "%d", (int) option);
      write(pipe1[1], buffer, 1024);

      if (option == PLANT_TREE)
        init_plant_tree(&pipe1[1]);
    }

    close(pipe1[1]); // close pipe
  } else {
    close(pipe1[1]);  // close write end of the pipe

    while (true) {
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
        exit(3);
      }

      printf("[+] TCP server socket created. \n");

      // define the server address
      memset(&addr, '\0', sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(ip);

      if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("[-] Connection error!");
        exit(4);
      }

      printf("Connected to the server. \n");

      // send the message to the server socket
      bzero(buffer, 1024);
      read(pipe1[0], buffer, 1024);
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

    close(pipe1[0]); // close pipe
  }

  return 0;
}
