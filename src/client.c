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

#define BUFSIZE 1024

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
      char buffer[BUFSIZE];
      Tree* t = malloc(sizeof(Tree));
      MenuOption option;
      moption_display(&option);

      if (moption_handle(option, t) == -1) {
        perror("[-] Menu Option error!");
        continue;
      }

      bzero(buffer, BUFSIZE);
      tree_serialise(t, buffer);
      write(pipe1[1], buffer, BUFSIZE);
    }

    close(pipe1[1]);  // clean up
  } else {
    close(pipe1[1]);  // close the write end of the pipe

    while (true) {
      // user-defined server's ip and port
      /* Make sure both the client and server are having the same ip and port */
      char ip[] = "127.0.0.1";
      int port = 3939;

      int sock;
      struct sockaddr_in addr;
      // socklen_t addr_size;

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
      char* buf = malloc(BUFSIZE);
      bzero(buf, BUFSIZE);
      read(pipe1[0], buf, BUFSIZE);
      send(sock, buf, BUFSIZE, 0);

      // receive the message from the server socket
      bzero(buf, BUFSIZE);
      recv(sock, buf, BUFSIZE, 0);
      printf("Server: %s\n", buf);

      free(buf);
      // close the connection (socket)
      close(sock);
      printf("Disconnected from the server.\n");
    }

    close(pipe1[0]);  // clean up
  }

  return 0;
}
