/* client.c */
#include "inet.h"
#include <error.h>
#include <string.h>
#include <strings.h> //bzero requires
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h> /* gethostbyname() */
#define PORT 3490
#define BUFSIZE 1024 // for recv

#define BUFFER_SIZE 512 // for send

int main(int argc, char const *argv[])
{
    int sockfd, numbytes;
    char buffer[BUFFER_SIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;

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

    int result = 1;
    do
    {
        // send coordinates
        printf("Enter the X Coordinates of the tree: ");
        scanf("%s", buffer);
        send(sockfd, buffer, sizeof(buffer), 0);
        printf("Enter the Y Coordinates of the tree: ");
        scanf("%s", buffer);
        send(sockfd, buffer, sizeof(buffer), 0);

        recv(sockfd, buffer, sizeof(buffer), 0);
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
    // convert int to string
    sprintf(buffer, "%d", speciesChosen);
    // send the string to the server
    send(sockfd, buffer, sizeof(buffer), 0);
    bzero(buffer, sizeof(buffer));

    // send the date to the server
    printf("Enter the date planted in this format dd/mm/yy: \n");
    scanf("%s", buffer);
    send(sockfd, buffer, sizeof(buffer), 0);

    // check user input
    do
    {
        printf("Enter the status of the tree (1) Alive (2) Dead: \n");
        scanf("%d", &state);
    } while (state > 2 || state < 1);

    // convert int to string
    sprintf(buffer, "%d", state);
    // bzero(buffer, sizeof(buffer));
    // send the string to the server
    send(sockfd, buffer, sizeof(buffer), 0);

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