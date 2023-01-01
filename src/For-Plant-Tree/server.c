/* server.c */
#include "inet.h"
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <signal.h>

#define MYPORT 3490
#define BACKLOG 5
#define BUFSIZE 1024 // for recv

#define MAX_TREES 100
#define BUFFER_SIZE 512 // for send

struct details
{
    int vertical;   /* x location*/
    int horizontal; /* y location*/
    char species[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char status[BUFFER_SIZE];
} tree, trees[MAX_TREES];

struct details get_coordinates(struct details tree, int sockfd, char *buffer);
int check_tree_exist(char *filename, struct details tree, struct details trees[], int *treeIndex);
void update_tree(char *filename, struct details trees[], int *treeIndex, int sockfd, char *buffer);
void write_tree(char *filename, struct details trees[], int treeIndex);

void sigchld_handler(int sig);

int main(int argc, char const *argv[])
{
    char *filename = "test.txt";
    int find_result = 0;
    int treeIndex = -1;

    int sockfd, new_sockfd, len_size;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    char buffer[BUFFER_SIZE];
    struct sigaction sa;
    int yes = 1; /* for setsockopt */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\nsocket() error!!!\n");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("\nsetsockopt() error!!!\n");
        exit(1);
    }

    bzero((char *)&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;     /* host byte order */
    my_addr.sin_port = htons(MYPORT); /* short, network byte order */

    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("\nbind() error!!!\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("\nlisten() error \n");
        exit(1);
    }

    printf("\nWaiting for connection!!!\n");

    // sa.sa_handler = sigchld_handler; /* reap all dead processes */
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = SA_RESTART; /* restart system call when there is interrupt */
    // if (sigaction(SIGCHLD, &sa, (int *)0) == -1)
    // {
    //     printf("\nsigaction() error!!!\n");
    //     exit(1);
    // }

    for (;;) /* main accept() loop */
    {
        sin_size = sizeof(struct sockaddr_in);
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            printf("\naccept() error!!!\n");
            exit(1);
        }
        printf("Server: connection from %s \n", inet_ntoa(their_addr.sin_addr));
        len_size = sizeof(struct sockaddr_in);
        getsockname(sockfd, (struct sockaddr *)&my_addr, &len_size);
        printf("\n %s", inet_ntoa(my_addr.sin_addr));
        len_size = sizeof(struct sockaddr_in);
        getpeername(sockfd, (struct sockaddr *)&their_addr, &len_size);
        printf("\n %s", inet_ntoa(their_addr.sin_addr));

        if (!fork()) /* !1 means 0, child process */
        {
            close(sockfd);
            do
            {
                tree = get_coordinates(tree, new_sockfd, buffer);
                find_result = check_tree_exist(filename, tree, trees, &treeIndex);

                if (find_result)
                    send(new_sockfd, "1", sizeof(buffer), 0);

                else
                    send(new_sockfd, "0", sizeof(buffer), 0);

            } while (find_result);

            printf("\nTree coordinates: %d, %d", tree.vertical, tree.horizontal);

            update_tree(filename, trees, &treeIndex, new_sockfd, buffer);

            // printf("\nSending Hello, world! to %s via socket %d \n", inet_ntoa(their_addr.sin_addr), new_sockfd);
            close(new_sockfd);
            exit(0);
        }
        close(new_sockfd);
    }
    return 0;
}

// void sigchld_handler(int sig)
// {
//     wait((int *)0);
// }

struct details get_coordinates(struct details tree, int sockfd, char *buffer)
{
    int num;
    // recv // tree.vertical
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    num = atoi(buffer);
    tree.vertical = num;
    // recv // tree.horizontal
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    num = atoi(buffer);
    tree.horizontal = num;
    return tree;
}

int check_tree_exist(char *filename, struct details tree, struct details trees[], int *treeIndex)
{
    FILE *fp;

    // variable INIT
    int numTrees = 0;
    char temp[BUFFER_SIZE];

    // Open file for reading
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error opening the file %s for reading", filename);
        exit(0);
    }

    // Read tree data from the file and store in the trees array
    while (fgets(temp, 512, fp) != NULL)
    {
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &trees[numTrees].vertical, &trees[numTrees].horizontal, trees[numTrees].species, trees[numTrees].date, trees[numTrees].status);
        if (trees[numTrees].vertical == tree.vertical && trees[numTrees].horizontal == tree.horizontal)
        {
            *treeIndex = numTrees;
            // treeIndex can be used to show which line is the search tree located in the textfile;
            printf("The coordinates found was located on line %d\n", *treeIndex + 1);
            return 1;
        }
        numTrees++;
    }
    // initialize trees[*treeIndex].xxxxx  to be used when writing
    trees[*treeIndex].vertical = tree.vertical;
    trees[*treeIndex].horizontal = tree.horizontal;
    fclose(fp);

    return 0;
}

void update_tree(char *filename, struct details trees[], int *treeIndex, int sockfd, char *buffer)
{
    // variable INIT
    int speciesChosen = 0;
    int state = 0;

    // check user input
    // get species
    bzero(buffer, sizeof(buffer));
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    speciesChosen = atoi(buffer);

    if (speciesChosen == 1)
        strncpy(trees[*treeIndex].species, "Deciduous", 10);
    else
        strncpy(trees[*treeIndex].species, "Coniferous", 11);

    // get date
    bzero(buffer, sizeof(buffer));
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    strncpy(trees[*treeIndex].date, buffer, sizeof(buffer));

    // check user input
    // get status
    bzero(buffer, sizeof(buffer));
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    state = atoi(buffer);
    if (state == 1)
        strncpy(trees[*treeIndex].status, "Alive", 6);
    else
        strncpy(trees[*treeIndex].status, "Dead", 5);

    printf("\nCheckpoint!--\n");
    write_tree(filename, trees, *treeIndex);
}

void write_tree(char *filename, struct details trees[], int treeIndex)
{
    /* open file for writing */
    FILE *of;
    of = fopen(filename, "a");

    if (of == NULL)
    {
        printf("Error opening the file %s", filename);
        exit(0);
    }

    // append the array data for trees[treeIndex] into the text file
    fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);

    fclose(of);
}