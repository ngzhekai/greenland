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

  for (;;) {
    /* main accept() loop */
    socklen_t sin_size = sizeof(struct sockaddr_in);

    if ((new_sockfd = accept(sockfd, (struct sockaddr*)&their_addr,
                             &sin_size)) == -1) {
      printf("\naccept() error!!!\n");
      exit(1);
    }

    printf("Connection from %s in socket %d accepted\n",
           inet_ntoa(their_addr.sin_addr),
           new_sockfd);
    socklen_t len_size = sizeof(struct sockaddr_in);
    getsockname(sockfd, (struct sockaddr*)&my_addr, &len_size);
    printf("\nServer address: %s\n", inet_ntoa(my_addr.sin_addr));
    len_size = sizeof(struct sockaddr_in);
    getpeername(sockfd, (struct sockaddr*)&their_addr, &len_size);
    char* cli_addr = inet_ntoa(their_addr.sin_addr);
    printf("Client address: %s\n", cli_addr);

    if (!fork()) {
      /* !1 means 0, child process  (if it is confusing 0 is equivalent to false) */
      close(sockfd);
      int semid;
      // pid_t pid = getpid();

      if ((semid = initsem(semkey)) <
          0) {
        /* call the initsem() function and if return value is less than 0 shows semget fails. */
        exit(1);
      }

      MenuOption client_menuoption;

      do {
        // receive option from client
        recv(new_sockfd, buffer, BUFFER_SIZE, 0);
        client_menuoption = strtol(buffer, NULL, 10);
        printf("\nClient [%s] requested option %d [%s]\n", cli_addr, client_menuoption,
               getMenuOptionName(client_menuoption));

        // handling menu option
        bzero(buffer, 1024);
        snprintf(buffer, 1024, "%s\n", moption_handle(new_sockfd, semid, cli_addr,
                 client_menuoption));
        printf("\n%s\n", buffer);
      } while (client_menuoption != EXIT_PROGRAM);

      close(new_sockfd);
      exit(0);
    }

    // wait((int *)0);
    close(new_sockfd);
  }

  return 0;
}

int initsem(key_t semkey)
{
  int status = 0, semid;
  semun arg;

  if ((semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL)) == -1) {
    /* create semaphore using semget() function with the key (semkey) and set 1 semaphore in the set.
      Flags:
      - SEMPERM -> file Permission of 0660
      - IPC_CREAT -> create semaphore if not yet exist
      - IPC_EXCL ->  return -1 with errno EEXIST if semaphore already exist (basically it causes the semget() function to fail if semaphore already exist)
      */
    if (errno ==
        EEXIST) {
      /* check if error/failure of semget() fuction is because of semaphore already exists. */
      semid = semget(semkey, 1, 0);
    } /* access to semaphore with key (semkey) and with 1 semaphore in teh set

                                            and the last argument '0' which is the semflg that apply the following:
                                                + If a semaphore identifier has already been created with key earlier, and the calling process of this semget() has read and/or write permissions to it, then semget() returns the associated semaphore identifier.
                                                + If a semaphore identifier has already been created with key earlier, and the calling process of this semget() does not have read and/or write permissions to it, then semget() returns-1 and sets errno to EACCES.
                                                + If a semaphore identifier has not been created with key earlier, then semget() returns -1 and sets errno to ENOENT.
                                            [Refer here for more info](https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-semget-get-set-semaphores)
                                            */
  } else {
    arg.val = 1; /* initialize arg.val to 1 */
    status = semctl(semid, 0, SETVAL,
                    arg); /* set the semaphore value (semval) of semaphore number '0' with id equals semid to 1 (arg) using SETVAL */
  }

  if (semid == -1 || status == -1) {
    perror("Initsem() fails");
    return (-1);
  }

  return (semid);
} /* end of initsem() function */