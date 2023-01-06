/* server.c */
#include "inet.h"
#include "sem.h"
#include "../lib/menuoption.h"
#include "../lib/tree.h"
#include "../lib/tree_coordinate.h"
#include "../lib/tree_state.h"

#define MYPORT 3939
#define BACKLOG 5
#define MAX_TREES 100

/* semaphore function declaration */
int initsem(key_t semkey);
int p(int semid);
int v(int semid);

/* function declaration */
// tree_coordinate get_coordinates(int sockfd, char* buffer);
// int check_tree_exist(const char* filename, Tree* tree, int* treeIndex,
//                      tree_coordinate* c, tree_coordinate* store);
// void update_detail(const char* filename, Tree tree, int sockfd, char* buffer,
//                    tree_coordinate store);
// void copy_tree(const char* filename, tree_coordinate store);
// void plant_tree(const char* filename, Tree tree, int new_sockfd, char* buffer);
// void update_tree(const char* filename, Tree tree, int new_sockfd,
//                  char* buffer);
// void query_tree(const char* filename, Tree tree, int new_sockfd, char* buffer);

// void sigchld_handler(int sig);

int main(int argc, char const* argv[])
{
  key_t semkey = 0x200; /* set value of IPC key in hexadecimal (0x200) */

  int sockfd, new_sockfd;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr;
  char buffer[BUFFER_SIZE];

  // struct sigaction sa;

  int yes = 1; /* for setsockopt */

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("\nsocket() error!!!\n");
    exit(1);
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("\nsetsockopt() error!!!\n");
    exit(1);
  }

  bzero((char*)&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;     /* host byte order */
  my_addr.sin_port = htons(MYPORT); /* short, network byte order */

  my_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(my_addr.sin_zero), 8);

  if (bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
    perror("\nbind() error!!!\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("\nlisten() error \n");
    exit(1);
  }

  printf("\nWaiting for connection!!!\n");

  // from lab 9 helloserver.c - SIGCHLD signal used to clean up child processes that have terminated.
  // sa.sa_handler = sigchld_handler; /* reap all dead processes */
  // sigemptyset(&sa.sa_mask);
  // sa.sa_flags = SA_RESTART; /* restart system call when there is interrupt, This can be useful for a server program that needs to maintain a connection with a client and does not want the system call to be interrupted by the SIGCHLD signal */
  // if (sigaction(SIGCHLD, &sa, (struct sigaction *)0) == -1)
  // {
  //     printf("\nsigaction() error!!!\n");
  //     exit(1);
  // }

  for (;;) { /* main accept() loop */
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((new_sockfd = accept(sockfd, (struct sockaddr*)&their_addr,
                             &sin_size)) == -1) {
      printf("\naccept() error!!!\n");
      exit(1);
    }

    printf("Connection from %s in socket %d accepted\n", inet_ntoa(their_addr.sin_addr),
           new_sockfd);
    socklen_t len_size = sizeof(struct sockaddr_in);
    getsockname(sockfd, (struct sockaddr*)&my_addr, &len_size);
    printf("\nServer address: %s\n", inet_ntoa(my_addr.sin_addr));
    len_size = sizeof(struct sockaddr_in);
    getpeername(sockfd, (struct sockaddr*)&their_addr, &len_size);
    char* cli_addr = inet_ntoa(their_addr.sin_addr);
    printf("Client address: %s\n", cli_addr);

    if (!fork()) { /* !1 means 0, child process  (if it is confusing 0 is equivalent to false) */
      close(sockfd);
      int semid;
      // pid_t pid = getpid();

      if ((semid = initsem(semkey)) <
          0) { /* call the initsem() function and if return value is less than 0 shows semget fails. */
        exit(1);
      }

      // receive option from client
      recv(new_sockfd, buffer, BUFFER_SIZE, 0);

      MenuOption client_menuoption = strtol(buffer, NULL, 10);
      printf("\nClient [%s] Requested Option %d [%s]\n", cli_addr, client_menuoption,
             getMenuOptionName(client_menuoption));

      // handling menu option
      bzero(buffer, 1024);
      snprintf(buffer, 1024, "%s", moption_handle(new_sockfd, semid, cli_addr,
               client_menuoption));

      // send the message to the client socket
      printf("\n%s\n", buffer);
      close(new_sockfd);
      exit(0);
    }

    // wait((int *)0);
    close(new_sockfd);
  }

  return 0;
}