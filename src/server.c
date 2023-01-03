/* server.c */
#include "inet.h"
#include "sem.h"
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <signal.h>
#include "../lib/menuoption.h"

#define MYPORT 3939
#define BACKLOG 5
#define BUFSIZE 1024

#define MAX_TREES 100
#define BUFFER_SIZE 512 // for send and recv (must be same)

struct details
{
    int vertical;   /* x location*/
    int horizontal; /* y location*/
    char species[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char status[BUFFER_SIZE];
} tree, trees[MAX_TREES];

/* semaphore function declaration */
int initsem(key_t semkey);
int p(int semid);
int v(int semid);

/* function declaration */
struct details get_coordinates(struct details tree, int sockfd, char *buffer);
int check_tree_exist(char *filename, struct details tree, struct details trees[], int *treeIndex);
void update_detail(char *filename, struct details trees[], int *treeIndex, int sockfd, char *buffer);
void write_tree(char *filename, struct details trees[], int treeIndex);
void copy_tree(char *filename, struct details *trees, struct details tree, int *treeIndex);
void plant_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer);
void update_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer);
void query_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer);

void sigchld_handler(int sig);

int main(int argc, char const *argv[])
{
    key_t semkey = 0x200; /* set value of IPC key in hexadecimal (0x200) */

    char *filename = "test.txt";

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

    // from lab 9 helloserver.c - SIGCHLD signal used to clean up child processes that have terminated.
    sa.sa_handler = sigchld_handler; /* reap all dead processes */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* restart system call when there is interrupt, This can be useful for a server program that needs to maintain a connection with a client and does not want the system call to be interrupted by the SIGCHLD signal */
    if (sigaction(SIGCHLD, &sa, (struct sigaction *)0) == -1)
    {
        printf("\nsigaction() error!!!\n");
        exit(1);
    }

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
        printf("\nServer address: %s\n", inet_ntoa(my_addr.sin_addr));
        len_size = sizeof(struct sockaddr_in);
        getpeername(sockfd, (struct sockaddr *)&their_addr, &len_size);
        printf("Client address: %s\n", inet_ntoa(their_addr.sin_addr));

        if (!fork()) /* !1 means 0, child process  (if it is confusing 0 is equivalent to false) */
        {
            close(sockfd);

            int semid;
            pid_t pid = getpid();

            if ((semid = initsem(semkey)) < 0) /* call the initsem() function and if return value is less than 0 shows semget fails. */
                exit(1);

            // receive option from client
            recv(new_sockfd, buffer, BUFFER_SIZE, 0);
            int option = atoi(buffer);
            // printf("\n%d", option); //for debug

            switch (option)
            {
            case 1:
                // printf("\n Process %d before entering critical section\n", pid); // debug
                p(semid); // lock the semaphore for writing

                /* critical section */

                // printf("\nProcess %d in critical section now\n", pid); // debug
                /* invoke plant_tree function */
                plant_tree(filename, tree, trees, new_sockfd, buffer);
                // printf("\nProcess %d leaving critical section\n", pid); // debug
                v(semid); // unlock the semaphore after writing
                // printf("\nProcess %d end \n", pid);                     // debug
                break;

            case 2:
                /* invoke query_tree function */
                query_tree(filename, tree, trees, new_sockfd, buffer);
                break;

            case 3:
                p(semid); // lock the semaphore for writing
                /* critical section */
                /* invoke update_tree function */
                update_tree(filename, tree, trees, new_sockfd, buffer);
                v(semid); // unlock the semaphore after writing
                break;

            default:
                break;
            }

            // printf("\nSending Hello, world! to %s via socket %d \n", inet_ntoa(their_addr.sin_addr), new_sockfd);
            close(new_sockfd);
            exit(0);
        }
        close(new_sockfd);
    }
    return 0;
}

void sigchld_handler(int sig)
{
    wait((int *)0);
}

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
} /* end of get_coordinates() function */

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
    // initialize trees[*treeIndex].xxxxx  to be used when writing (which is mainly for creating a new tree)
    trees[*treeIndex].vertical = tree.vertical;
    trees[*treeIndex].horizontal = tree.horizontal;
    fclose(fp);

    return 0;
} /* end of check_tree_exist() function */

void update_detail(char *filename, struct details trees[], int *treeIndex, int sockfd, char *buffer)
{
    // variable INIT
    int speciesChosen = 0;
    int state = 0;

    // get species from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    speciesChosen = atoi(buffer);

    if (speciesChosen == 1)
        strncpy(trees[*treeIndex].species, "Deciduous", 10);
    else
        strncpy(trees[*treeIndex].species, "Coniferous", 11);

    // get date from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    strncpy(trees[*treeIndex].date, buffer, BUFFER_SIZE);

    // get status from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    state = atoi(buffer);
    if (state == 1)
        strncpy(trees[*treeIndex].status, "Alive", 6);
    else
        strncpy(trees[*treeIndex].status, "Dead", 5);

    write_tree(filename, trees, *treeIndex);
    printf("\n<-- debug Checkpoint! (writing completed from update_details() func) -->\n");
} /* end of update_detail() function */

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
} /* end of write_tree() function */

void copy_tree(char *filename, struct details *trees, struct details tree, int *treeIndex)
{
    FILE *fp;

    // Open file for reading
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error opening the file %s for reading", filename);
        exit(0);
    }

    // variable INIT
    int numTrees = 0;
    char temp[BUFFER_SIZE];

    // Open file for copying into temporary text file
    FILE *tmp;
    tmp = fopen("temp.txt", "a");
    if (tmp == NULL)
    {
        printf("Error opening the file %s for temporary writing", filename);
        exit(0);
    }

    // Read tree data from the file and store in the trees array
    while (fgets(temp, 512, fp) != NULL)
    {
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &trees[numTrees].vertical, &trees[numTrees].horizontal, trees[numTrees].species, trees[numTrees].date, trees[numTrees].status);
        if (!(trees[numTrees].vertical == tree.vertical && trees[numTrees].horizontal == tree.horizontal))
        {
            fprintf(tmp, "%d,%d\t\t%s\t\t%s\t\t%s\n", trees[numTrees].vertical, trees[numTrees].horizontal, trees[numTrees].species, trees[numTrees].date, trees[numTrees].status);
        }
        numTrees++;
    }

    fclose(tmp);
    fclose(fp);

    // rename temp.txt to test.txt
    if (rename("temp.txt", filename) == 0)
    {
        // printf("Copied successfully\n"); // debug purpose only
        unlink("temp.txt");
    }
    else
    {
        perror("Error: has occurred\n");
        exit(1);
    }
} /* end of copy_tree() function */

void plant_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer)
{

    int find_result = 0;
    int treeIndex = -1;
    do
    {
        tree = get_coordinates(tree, new_sockfd, buffer);
        find_result = check_tree_exist(filename, tree, trees, &treeIndex);

        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (find_result);

    // printf("\nTree coordinates: %d, %d", tree.vertical, tree.horizontal); // for debug

    /* critical section */
    update_detail(filename, trees, &treeIndex, new_sockfd, buffer);
    /* end of critical section */

    // printf("This is the newly created tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug
    // printf("-----------\t-------\t\t----\t\t------\n");
    sprintf(buffer, "%d,%d\t\t%s\t%s\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);
    // printf("%s\n", buffer); // for debug
    send(new_sockfd, buffer, BUFFER_SIZE, 0);

} /* end of plant_tree() function */

void update_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer)
{

    int find_result = 0;
    int treeIndex = -1;
    do
    {
        tree = get_coordinates(tree, new_sockfd, buffer);
        find_result = check_tree_exist(filename, tree, trees, &treeIndex);

        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (!find_result);

    // printf("This is the current tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug
    // printf("-----------\t-------\t\t----\t\t------\n");
    sprintf(buffer, "%d,%d\t\t%s\t%s\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);
    // printf("%s\n", buffer); // for debug
    send(new_sockfd, buffer, BUFFER_SIZE, 0);

    /* critical section */
    copy_tree(filename, trees, tree, &treeIndex);
    update_detail(filename, trees, &treeIndex, new_sockfd, buffer);
    /* end of critical section */

    // printf("This is the updated tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");  // for debug
    // printf("-----------\t-------\t\t----\t\t------\n");
    sprintf(buffer, "%d,%d\t\t%s\t%s\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);
    // printf("%s\n", buffer); // for debug
    send(new_sockfd, buffer, BUFFER_SIZE, 0);

} /* end of update_tree() function */

void query_tree(char *filename, struct details tree, struct details trees[], int new_sockfd, char *buffer)
{
    int find_result = 0;
    int treeIndex = -1;

    do
    {
        tree = get_coordinates(tree, new_sockfd, buffer);
        find_result = check_tree_exist(filename, tree, trees, &treeIndex);

        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (!find_result);

    // printf("This is the query result:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug
    // printf("-----------\t-------\t\t----\t\t------\n");
    sprintf(buffer, "%d,%d\t\t%s\t%s\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);
    // printf("%s", buffer); // for debug
    send(new_sockfd, buffer, BUFFER_SIZE, 0);
} /* end of query_tree() function */

int initsem(key_t semkey)
{
    int status = 0, semid;
    semun arg;

    if ((semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL)) == -1)
    {                                     /* create semaphore using semget() function with the key (semkey) and set 1 semaphore in the set.
                                            Flags:
                                            - SEMPERM -> file Permission of 0660
                                            - IPC_CREAT -> create semaphore if not yet exist
                                            - IPC_EXCL ->  return -1 with errno EEXIST if semaphore already exist (basically it causes the semget() function to fail if semaphore already exist)
                                            */
        if (errno == EEXIST)              /* check if error/failure of semget() fuction is because of semaphore already exists. */
            semid = semget(semkey, 1, 0); /* access to semaphore with key (semkey) and with 1 semaphore in teh set
                                            and the last argument '0' which is the semflg that apply the following:
                                                + If a semaphore identifier has already been created with key earlier, and the calling process of this semget() has read and/or write permissions to it, then semget() returns the associated semaphore identifier.
                                                + If a semaphore identifier has already been created with key earlier, and the calling process of this semget() does not have read and/or write permissions to it, then semget() returns-1 and sets errno to EACCES.
                                                + If a semaphore identifier has not been created with key earlier, then semget() returns -1 and sets errno to ENOENT.
                                            [Refer here for more info](https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-semget-get-set-semaphores)
                                            */
    }
    else
    {
        arg.val = 1;                            /* initialize arg.val to 1 */
        status = semctl(semid, 0, SETVAL, arg); /* set the semaphore value (semval) of semaphore number '0' with id equals semid to 1 (arg) using SETVAL */
    }

    if (semid == -1 || status == -1)
    {
        perror("Initsem() fails");
        return (-1);
    }
    return (semid);
} /* end of initsem() function */

int p(int semid)
{
    struct sembuf p_buf;
    p_buf.sem_num = 0; /* semaphore number of the sepamore set,
                         value '0' shows the first semaphore in the set is chosen */

    p_buf.sem_op = -1; /* the semaphore value is decremented by the absolute value of sem_op (-1)
                         showing the locking operation of semaphore is done */

    p_buf.sem_flg = SEM_UNDO; /* SEM_UNDO operation flag is used to tell the system to undo the process's semaphore changes automaticall, when the process exits.
                                This allows processes to avoid deadlock problems. */

    if (semop(semid, &p_buf, 1) == -1)
    { /* add explanation here */
        perror("P () fails");
        exit(1);
    }
    return (0);
} /* end of sem p() */

int v(int semid)
{
    struct sembuf v_buf;
    v_buf.sem_num = 0; /* semaphore number of the sepamore set,
                         value '0' shows the first semaphore in the set is chosen */

    v_buf.sem_op = 1; /* the semaphore value is incremented by the absolute value of sem_op (1),
                        showing the unlocking operation of semaphore is done. */

    v_buf.sem_flg = SEM_UNDO; /* SEM_UNDO operation flag is used to tell the system to undo the process's semaphore changes automaticall, when the process exits.
                                This allows processes to avoid deadlock problems. */

    if (semop(semid, &v_buf, 1) == -1)
    { /* perform the semaphore operation as specified in v_buf struct on semaphore with the id (semid)
        the last argument '1' shows the number of sembuf structure in the array. */
        perror("V (semid) fails");
        exit(1);
    }
    return (0);
}