/* client.c */
#include "inet.h"
#include <error.h>
#include <string.h>
#include <strings.h> //bzero requires
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h> /* gethostbyname() */
#include "../lib/menuoption.h"
#define PORT 3939
#define BUFSIZE 1024 //

#define BUFFER_SIZE 512 // for send and recv (must be same)

/* function declaration */
int menu_display();
void plant_tree(int sockfd, char *buffer);
void update_tree(int sockfd, char *buffer);
void query_tree(int sockfd, char *buffer);

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

int main(int argc, char const *argv[])
{
    int sockfd, numbytes;
    char buffer[BUFFER_SIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;

    sigset_t set1;
    sigemptyset(&set1);        // initialize the signal set with an empty set of signals
    sigaddset(&set1, SIGTSTP); // add ctrl+z to the signal set
    sigaddset(&set1, SIGINT);  // add ctrl+c to the signal set
    // sigfillset(&set1); // debug

    if (argc != 2)
    {
        fprintf(stderr, "\nHow to use: ./client RemoteIPaddress!!!\n");
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == (void *)0) /*
                                                    check if the gethostbyname() function returns null
                                                    What will gethostbyname() function returns:
                                                    Return entry from host data base for host using the supplied argument [NAME]
                                                    */
    {
        printf("\ngethostbyname() error!!!\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("\nsocket() error!!!\n");
        exit(1);
    }

    bzero(&(their_addr), sizeof(their_addr));
    their_addr.sin_family = AF_INET;   /* host byte order */
    their_addr.sin_port = htons(PORT); /* short, network byte order */

    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("\nconnect() error !!!\n");
        exit(1);
    }

    sigprocmask(SIG_SETMASK, &set1, NULL); // activates the signal block
    int option = menu_display();

    switch (option)
    {
    case 1:
        // invoke plant_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        plant_tree(sockfd, buffer);
        break;

    case 2:
        // invoke query_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        query_tree(sockfd, buffer);
        break;

    case 3:
        // invoke update_tree() method
        sprintf(buffer, "%d", option); // convert int (option) to string
        // send the string to the server
        send(sockfd, buffer, BUFFER_SIZE, 0);
        update_tree(sockfd, buffer);
        break;

    case 4:
        exit(0);

    default:
        printf("You entered %d! Please enter 1, 2, or 3 Only!\n", option);
        break;
    }
    sigprocmask(SIG_UNBLOCK, &set1, NULL); // activates the signal block

    // if ((numbytes = recv(sockfd, buffer, MAXDATASIZE, 0)) == -1)
    // {
    //     printf("\nrecv() error!!!\n");
    //     exit(1);
    // }

    // buffer[numbytes] = '\0';
    // printf("\nReceived : %s", buffer);
    close(sockfd);
    return 0;
}

/* Function starts here */

void plant_tree(int sockfd, char *buffer)
{
    int result = 1;
    do
    {
        // send coordinates
        printf("Enter the X Coordinates of the tree: ");
        scanf("%s", buffer);
        send(sockfd, buffer, BUFFER_SIZE, 0);
        printf("Enter the Y Coordinates of the tree: ");
        scanf("%s", buffer);
        send(sockfd, buffer, BUFFER_SIZE, 0);

        recv(sockfd, buffer, BUFFER_SIZE, 0);
        result = atoi(buffer);
        if (result)
            printf("\nA tree was found in the database at the given location.\nTry a new coordinate!\n\n\n");

    } while (result);

    // variable INIT
    int speciesChosen = 0;
    int state = 0;

    // check user input is valid before sending to the server
    do
    {
        printf("Enter the species of the tree (1) Deciduous (2) Coniferous: \n");
        scanf("%d", &speciesChosen);
    } while (speciesChosen > 2 || speciesChosen < 1);
    // convert int to string and store in buffer
    sprintf(buffer, "%d", speciesChosen);
    // send the string to the server
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // send the date to the server
    printf("Enter the date planted in this format dd/mm/yyyy: \n");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // check user input
    do
    {
        printf("Enter the status of the tree (1) Alive (2) Dead: \n");
        scanf("%d", &state);
    } while (state > 2 || state < 1);

    // convert int to string
    sprintf(buffer, "%d", state);
    // send the string to the server
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // print the updated tree details to the user.
    printf("\n\nThis is the newly created tree details:\n\n");
    printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
    printf("-----------\t-------\t\t----\t\t------\n");
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);

} /* end of plant_tree() function */

void update_tree(int sockfd, char *buffer)
{
    int result = 0;
    do
    {
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
        if (!result)
            printf("\nNo tree with the given coordinates was found in the database.\nTry Again!\n\n\n");

    } while (!result);

    // print the current tree details to the user.
    printf("\n\nThis is the current tree details:\n\n");
    printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
    printf("-----------\t-------\t\t----\t\t------\n");
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);

    // remark:copy tree should not be performed on the client end
    // variable INIT
    int speciesChosen = 0;
    int state = 0;

    // check user input is valid before sending to the server
    do
    {
        printf("Enter the species of the tree (1) Deciduous (2) Coniferous: \n");
        scanf("%d", &speciesChosen);
    } while (speciesChosen > 2 || speciesChosen < 1);

    // convert int to string
    sprintf(buffer, "%d", speciesChosen);
    // send the string to the server
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // send the date to the server
    printf("Enter the date planted in this format dd/mm/yyyy: \n");
    scanf("%s", buffer);
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // check user input
    do
    {
        printf("Enter the status of the tree (1) Alive (2) Dead: \n");
        scanf("%d", &state);
    } while (state > 2 || state < 1);

    // convert int to string
    sprintf(buffer, "%d", state);
    // send the string to the server
    send(sockfd, buffer, BUFFER_SIZE, 0);

    // print the updated tree details to the user.
    printf("\n\nThis is the updated tree details:\n\n");
    printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
    printf("-----------\t-------\t\t----\t\t------\n");
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s\n", buffer);

} /* end of update_tree() function */

void query_tree(int sockfd, char *buffer)
{
    int result = 0;
    do
    {
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
        if (!result)
            printf("\nNo tree with the given coordinates was found in the database.\nTry Again!\n\n\n");

    } while (!result);

    printf("\n\nThis is the query result:\n\n");
    printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");
    printf("-----------\t-------\t\t----\t\t------\n");
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);
}

int menu_display()
{
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
    printf("\t4. Exit\n");
    printf("(Choose a service needed according to the number)\n\n");
    printf("Option: ");
    int option;
    scanf("%d", &option);
    system("clear");
    return option;
} /* end of menu_display() function */

