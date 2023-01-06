/* client.c */
#include "inet.h"
#include "../lib/menuoption.h"
#define PORT 3939

/* function declaration */
void plant_tree(int sockfd, char* buffer);
void update_tree(int sockfd, char* buffer);
void query_tree(int sockfd, char* buffer);

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

int main(int argc, char const* argv[])
{
  int sockfd;
  // int numbytes;

  char buffer[BUFFER_SIZE];
  struct hostent* he;
  struct sockaddr_in their_addr;

  // sigset_t set1;
  // sigemptyset(&set1);        // initialize the signal set with an empty set of signals
  // sigaddset(&set1, SIGTSTP); // add ctrl+z to the signal set
  // sigaddset(&set1, SIGINT);  // add ctrl+c to the signal set
  // sigfillset(&set1); // debug

  if (argc != 2) {
    fprintf(stderr, "\nHow to use: ./client RemoteIPaddress!!!\n");
    exit(1);
  }

  if ((he = gethostbyname(argv[1])) == (void*)0) /*
                                                          check if the gethostbyname() function returns null
                                                          What will gethostbyname() function returns:
                                                          Return entry from host data base for host using the supplied argument [NAME]
                                                          */
  {
    printf("\ngethostbyname() error!!!\n");
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("\nsocket() error!!!\n");
    exit(1);
  }

  bzero(&(their_addr), sizeof(their_addr));
  their_addr.sin_family = AF_INET;   /* host byte order */
  their_addr.sin_port = htons(PORT); /* short, network byte order */

  their_addr.sin_addr = *((struct in_addr*)he->h_addr_list[0]);
  bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */

  if (connect(sockfd, (struct sockaddr*)&their_addr,
              sizeof(struct sockaddr)) == -1) {
    printf("\nconnect() error !!!\n");
    exit(1);
  }

  // sigprocmask(SIG_SETMASK, &set1, NULL); // activates the signal block
  MenuOption option;

  while (1) {
    moption_display(&option);

    switch (option) {
      case 1:
        // invoke plant_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        plant_tree(sockfd, buffer);
        printf("\n%s Operation Success!\n", getMenuOptionName(option));

        while ('\n' != getchar())
          ;

        printf("Press [ENTER] to return to menu...\n");
        getchar();
        system("clear");
        break;

      case 2:
        // invoke query_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        query_tree(sockfd, buffer);
        printf("\n%s Operation Success!\n", getMenuOptionName(option));

        while ('\n' != getchar())
          ;

        printf("Press [ENTER] to return to menu...\n");
        getchar();
        system("clear");
        break;

      case 3:
        // invoke update_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        update_tree(sockfd, buffer);
        printf("\n%s Operation Success!\n", getMenuOptionName(option));

        while ('\n' != getchar())
          ;

        printf("Press [ENTER] to return to menu...\n");
        getchar();
        system("clear");
        break;

      case 4:
        // invoke display_all_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        char* temp = 0;
        send(sockfd, buffer, BUFFER_SIZE, 0);
        recv(sockfd, temp, BUFFER_SIZE, 0);
        printf("%s\n", temp);

        while ('\n' != getchar())
          ;

        printf("Press [ENTER] to return to menu...\n");
        getchar();
        system("clear");
        break;

      case 5:
        sprintf(buffer, "%d", option); // convert int (option) to string
        send(sockfd, buffer, BUFFER_SIZE, 0);
        exit(0);

      default:
        printf("You entered %d! Please enter 1, 2, or 3 Only!\n", option);
        break;
    }
  }

  // sigprocmask(SIG_UNBLOCK, &set1, NULL); // activates the signal block
  close(sockfd);
  return 0;
}

/* Function starts here */

void plant_tree(int sockfd, char* buffer)
{
  int result = 1;

  do {
    // send coordinates
    printf("Enter the X Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);
    printf("Enter the Y Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);

    recv(sockfd, buffer, BUFFER_SIZE, 0);
    result = atoi(buffer);

    if (result) {
      printf("\nA tree was found in the database at the given location.\nTry a new coordinate!\n\n\n");
    }

  } while (result);

  // variable INIT
  int speciesChosen = 0;
  int state = 0;

  // check user input is valid before sending to the server
  do {
    printf("Enter the species of the tree (0) Deciduous (1) Coniferous: \n");
    scanf("%d", &speciesChosen);
  } while (speciesChosen > 1 || speciesChosen < 0);

  // convert int to string and store in buffer
  sprintf(buffer, "%d", speciesChosen);
  // send the string to the server
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // check user input
  do {
    printf("Enter the status of the tree (0) Dead (1) Alive (2) Sick (3) Treatment: \n");
    scanf("%d", &state);
  } while (state > 3 || state < 0);

  // convert int to string
  sprintf(buffer, "%d", state);

  // send the string to the server
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // send the date to the server
  printf("Enter the date planted in this format yyyy-mm-dd: \n");
  scanf("%s", buffer);
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // print the updated tree details to the user.
  printf("\n\nThis is the newly created tree details:\n\n");
  printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
  printf("-----------\t-------\t\t----\t\t------\n");
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  printf("%s\n", buffer);

} /* end of plant_tree() function */

void update_tree(int sockfd, char* buffer)
{
  int result = 0;

  do {
    // send coordinates
    printf("Enter the X Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);
    printf("Enter the Y Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // receive reply (result) from server
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    result = atoi(buffer);

    // Prompt the user (no tree located in the specified location)
    if (!result) {
      printf("\nNo tree with the given coordinates was found in the database.\nTry Again!\n\n\n");
    }

  } while (!result);

  // print the current tree details to the user.
  printf("\n\nThis is the current tree details:\n\n");
  printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // this features is discarded because having trouble to convert struct tm to strings
  printf("-----------\t-------\t\t----\t\t------\n");
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  printf("%s\n", buffer);

  // remark:copy tree should not be performed on the client end
  // variable INIT
  int speciesChosen = 0;
  int state = 0;

  // check user input is valid before sending to the server
  do {
    printf("Enter the species of the tree (0) Deciduous (1) Coniferous: \n");
    scanf("%d", &speciesChosen);
  } while (speciesChosen > 1 || speciesChosen < 0);

  // convert int to string
  sprintf(buffer, "%d", speciesChosen);
  // send the string to the server
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // check user input
  do {
    printf("Enter the status of the tree (0) Dead (1) Alive (2) Sick (3) Treatment: \n");
    scanf("%d", &state);
  } while (state > 3 || state < 0);

  // convert int to string
  sprintf(buffer, "%d", state);
  // send the string to the server
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // send the date to the server
  printf("Enter the date planted in this format yyyy-mm-dd: \n");
  scanf("%s", buffer);
  send(sockfd, buffer, BUFFER_SIZE, 0);

  // print the updated tree details to the user.
  printf("\n\nThis is the updated tree details:\n\n");
  printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
  printf("-----------\t-------\t\t----\t\t------\n");
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  printf("%s\n", buffer);

} /* end of update_tree() function */

void query_tree(int sockfd, char* buffer)
{
  int result = 0;

  do {
    // send coordinates
    printf("Enter the X Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);
    printf("Enter the Y Coordinates of the tree: ");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // receive reply (result) from server
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    result = atoi(buffer);

    // Prompt the user (no tree located in the specified location)
    if (!result) {
      printf("\nNo tree with the given coordinates was found in the database.\nTry Again!\n\n\n");
    }

  } while (!result);

  printf("\n\nThis is the query result:\n\n");
  printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
  printf("-----------\t-------\t\t----\t\t------\n");
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  printf("%s", buffer);
}