/* server.c */
#include "inet.h"
#include "sem.h"
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/wait.h>
#include <signal.h>
#include "../lib/menuoption.h"
#include "../lib/tree.h"
#include "../lib/tree_coordinate.h"
#include "../lib/tree_state.h"

#define MYPORT 3939
#define BACKLOG 5
#define BUFSIZE 1024

#define MAX_TREES 100
#define BUFFER_SIZE 512 // for send and recv (must be same)

/* semaphore function declaration */
int initsem(key_t semkey);
int p(int semid);
int v(int semid);

/* function declaration */
tree_coordinate get_coordinates(int sockfd, char *buffer);
int check_tree_exist(const char *filename, Tree *tree, int *treeIndex, tree_coordinate *c, tree_coordinate *store);
void update_detail(const char *filename, Tree tree, int sockfd, char *buffer, tree_coordinate store);
void copy_tree(const char *filename, tree_coordinate store);
void plant_tree(const char *filename, Tree tree, int new_sockfd, char *buffer);
void update_tree(const char *filename, Tree tree, int new_sockfd, char *buffer);
void query_tree(const char *filename, Tree tree, int new_sockfd, char *buffer);

void sigchld_handler(int sig);

int main(int argc, char const *argv[])
{
    key_t semkey = 0x200; /* set value of IPC key in hexadecimal (0x200) */

    const char *filename = "database.txt";

    int sockfd, new_sockfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    // int sin_size;
    // int len_size;
    char buffer[BUFFER_SIZE];

    // struct sigaction sa;

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
    // sa.sa_handler = sigchld_handler; /* reap all dead processes */
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = SA_RESTART; /* restart system call when there is interrupt, This can be useful for a server program that needs to maintain a connection with a client and does not want the system call to be interrupted by the SIGCHLD signal */
    // if (sigaction(SIGCHLD, &sa, (struct sigaction *)0) == -1)
    // {
    //     printf("\nsigaction() error!!!\n");
    //     exit(1);
    // }

    for (;;) /* main accept() loop */
    {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            printf("\naccept() error!!!\n");
            exit(1);
        }
        printf("Server: connection from %s \n", inet_ntoa(their_addr.sin_addr));
        socklen_t len_size = sizeof(struct sockaddr_in);
        getsockname(sockfd, (struct sockaddr *)&my_addr, &len_size);
        printf("\nServer address: %s\n", inet_ntoa(my_addr.sin_addr));
        len_size = sizeof(struct sockaddr_in);
        getpeername(sockfd, (struct sockaddr *)&their_addr, &len_size);
        printf("Client address: %s\n", inet_ntoa(their_addr.sin_addr));

        if (!fork()) /* !1 means 0, child process  (if it is confusing 0 is equivalent to false) */
        {
            close(sockfd);
            Tree tree;
            int semid;
            // pid_t pid = getpid();

            if ((semid = initsem(semkey)) < 0) /* call the initsem() function and if return value is less than 0 shows semget fails. */
                exit(1);

            // receive option from client
            recv(new_sockfd, buffer, BUFFER_SIZE, 0);
            int option = atoi(buffer);
            printf("\n%d\n", option); // for debug

            switch (option)
            {
            case 1:
                // printf("\n Process %d before entering critical section\n", pid); // debug
                p(semid); // lock the semaphore for writing

                /* critical section */

                // printf("\nProcess %d in critical section now\n", pid); // debug
                /* invoke plant_tree function */
                plant_tree(filename, tree, new_sockfd, buffer);
                // printf("\nProcess %d leaving critical section\n", pid); // debug
                v(semid); // unlock the semaphore after writing
                // printf("\nProcess %d end \n", pid);                     // debug
                break;

            case 2:
                /* invoke query_tree function */
                query_tree(filename, tree, new_sockfd, buffer);
                break;

            case 3:
                p(semid); // lock the semaphore for writing
                /* critical section */
                /* invoke update_tree function */
                update_tree(filename, tree, new_sockfd, buffer);
                v(semid); // unlock the semaphore after writing
                break;

            default:
                break;
            }
            // printf("\nSending Hello, world! to %s via socket %d \n", inet_ntoa(their_addr.sin_addr), new_sockfd);
            close(new_sockfd);
            exit(0);
        }
        // wait((int *)0);
        close(new_sockfd);
    }
    return 0;
}

void sigchld_handler(int sig)
{
    wait((int *)0);
}

tree_coordinate get_coordinates(int sockfd, char *buffer)
{
    tree_coordinate c;
    int num;
    // recv // tree.vertical
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    num = atoi(buffer);
    tree_set_x(&c, num);
    // recv // tree.horizontal
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    num = atoi(buffer);
    // tree.horizontal = num;
    tree_set_y(&c, num);
    return c;
} /* end of get_coordinates() function */

int check_tree_exist(const char *filename, Tree *tree, int *treeIndex, tree_coordinate *c, tree_coordinate *store)
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
        int vertical, horizontal;
        char species[BUFFER_SIZE], status[BUFFER_SIZE], date[BUFFER_SIZE];
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species, date, status);
        printf("x: %d, y: %d\n", vertical, horizontal);                         // debug
        printf("compare x: %d, compare y: %d\n", tree_get_x(c), tree_get_y(c)); // debug

        store->x = vertical;
        store->y = horizontal;
        tree_set_species(tree, species); // because species (sp) is a constant in the function so it will need to use tree_set_species()
        tree->status = atoi(status);
        tree->day_planted = NULL;         // initiaties the day_planted (always makes sure it is initiated)
        tree_set_day_planted(tree, date); //<---code got stuck here (fixed with initalization)

        printf("Debug: X coordinates print from Tree (store): %d\n", store->x); // debug
        printf("Debug: Y coordinates print from Tree (store): %d\n", store->y); // deubg
        if (vertical == tree_get_x(c) && horizontal == tree_get_y(c))
        {
            *treeIndex = numTrees;
            // treeIndex can be used to show which line is the search tree located in the textfile;
            printf("Debug: The coordinates found was located on line %d\n", *treeIndex + 1);
            return 1;
        }
        numTrees++;
    }
    // initialize trees[*treeIndex].xxxxx  to be used when writing (which is mainly for creating a new tree)
    // trees[*treeIndex].vertical = tree.vertical;
    // trees[*treeIndex].horizontal = tree.horizontal;
    printf("\nDebug Checkpoint: after searching\n");
    tree_set_x(store, tree_get_x(c));
    tree_set_y(store, tree_get_y(c));
    printf("\nDebug Checkpoint: end of check_tree_exist()\n");

    fclose(fp);

    return 0;
} /* end of check_tree_exist() function */

void update_detail(const char *filename, Tree tree, int sockfd, char *buffer, tree_coordinate store)
{
    // variable INIT
    int state = 0;
    int speciesChosen = 0;
    char sp[BUFSIZE];
    tree_state st;

    printf("\nDebug Checkpoint: beginning of update_detail function (recv from client)\n");

    // get species from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    speciesChosen = atoi(buffer);

    if (speciesChosen == 1)
    {
        // strncpy(trees[*treeIndex].species, "Deciduous", 10);
        tree_set_species(&tree, "Deciduous");
        strcpy(sp, "Deciduous");
    }
    else
    {
        // strncpy(trees[*treeIndex].species, "Coniferous", 11);
        tree_set_species(&tree, "Coniferous");
        strcpy(sp, "Coniferous");
    }
    printf("Species selected\n"); // debug

    // get status from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    state = atoi(buffer);
    if (state == 0)
        st = DEAD;
    else if (state == 1)
        st = PLANTED;
    else if (state == 2)
        st = SICK;
    else
        st = TREAMENT;
    tree.status = st;
    printf("state selected: %s\n", trstat_to_string(st)); // debug

    // get date from client
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("date received: %s\n", buffer);
    // strncpy(trees[*treeIndex].date, buffer, BUFFER_SIZE);
    char time[] = "2020-11-11"; // debug

    // char time[BUFFER_SIZE];
    // strncpy(time, buffer, BUFFER_SIZE);

    tree.day_planted = NULL;

    strncpy(time, buffer, BUFFER_SIZE);
    tree_set_day_planted(&tree, time);
    // tree.day_planted = time;
    printf("date selected\n");

    // Tree tree = *tree_create(sp, st, time, store);
    printf("\n before writing \n");
    // write_tree(filename, trees, *treeIndex, tree);

    FILE *of;
    of = fopen(filename, "a");

    if (of == NULL)
    {
        printf("Error opening the file %s", filename);
        exit(0);
    }

    printf("%d\n", store.x);
    printf("%d\n", store.y);
    printf("%s\n", tree.species);
    printf("%s\n", time);
    printf("%d\n", tree.status);
    fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%d\n", store.x, store.y, tree.species, time, tree.status);

    fclose(of);

    // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", store.x, store.y, tree.species, time, tree.status); //discarded to hardcode the status text
    // send back to the client
    if (tree.status == 0)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", store.x, store.y, tree.species, time);
    else if (tree.status == 1)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", store.x, store.y, tree.species, time);
    else if (tree.status == 2)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", store.x, store.y, tree.species, time);
    else
        sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", store.x, store.y, tree.species, time);

    send(sockfd, buffer, BUFFER_SIZE, 0);

    printf("\n<-- debug Checkpoint! (writing completed from update_details() func) -->\n");
} /* end of update_detail() function */

void copy_tree(const char *filename, tree_coordinate store)
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
        int vertical, horizontal;
        char species[BUFFER_SIZE], date[BUFFER_SIZE], status[BUFFER_SIZE];
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species, date, status);
        if (!(vertical == store.x && horizontal == store.y))
        {
            fprintf(tmp, "%d,%d\t\t%s\t\t%s\t\t%s\n", vertical, horizontal, species, date, status);
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

void plant_tree(const char *filename, Tree tree, int new_sockfd, char *buffer)
{

    tree_coordinate coordinates;
    int find_result = 0;
    int treeIndex = -1;
    tree_coordinate store;
    do
    {
        coordinates = get_coordinates(new_sockfd, buffer);
        find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates, &store);

        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (find_result);

    /* critical section */
    update_detail(filename, tree, new_sockfd, buffer, store);
    /* end of critical section */

    // printf("This is the newly created tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client end)
    // printf("-----------\t-------\t\t----\t\t------\n");

} /* end of plant_tree() function */

void update_tree(const char *filename, Tree tree, int new_sockfd, char *buffer)
{

    int find_result = 0;
    int treeIndex = -1;
    tree_coordinate coordinates;
    tree_coordinate store;

    do
    {
        coordinates = get_coordinates(new_sockfd, buffer);
        find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates, &store);

        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (!find_result);

    // printf("This is the current tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client ends)
    // printf("-----------\t-------\t\t----\t\t------\n");
    char date[BUFFER_SIZE];
    strftime(date, BUFFER_SIZE, "%Y-%m-%d", tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti
    // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", coordinates.x, coordinates.y, tree.species, date, tree.status);
    if (tree.status == 0)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y, tree.species, date);
    else if (tree.status == 1)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y, tree.species, date);
    else if (tree.status == 2)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y, tree.species, date);
    else
        sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x, coordinates.y, tree.species, date);

    send(new_sockfd, buffer, BUFFER_SIZE, 0);

    /* critical section */
    copy_tree(filename, store);
    update_detail(filename, tree, new_sockfd, buffer, store);
    /* end of critical section */

    // printf("This is the updated tree details:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");  // for debug (this is what it is going to show on the client end)
    // printf("-----------\t-------\t\t----\t\t------\n");

} /* end of update_tree() function */

void query_tree(const char *filename, Tree tree, int new_sockfd, char *buffer)
{
    int find_result = 0;
    int treeIndex = -1;
    tree_coordinate coordinates;
    tree_coordinate store;
    do
    {
        coordinates = get_coordinates(new_sockfd, buffer);
        find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates, &store);
        printf("findresult: %d", find_result);
        if (find_result)
            send(new_sockfd, "1", BUFFER_SIZE, 0);

        else
            send(new_sockfd, "0", BUFFER_SIZE, 0);

    } while (!find_result);

    // printf("This is the query result:\n\n");
    // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client end)
    // printf("-----------\t-------\t\t----\t\t------\n");
    // send to client
    char date[BUFFER_SIZE];
    strftime(date, BUFFER_SIZE, "%Y-%m-%d", tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti
    // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", coordinates.x, coordinates.y, tree.species, date, tree.status);
    printf("Debug: show tree.species: %s\n", tree_get_species(&tree));
    if (tree.status == 0)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y, tree.species, date);
    else if (tree.status == 1)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y, tree.species, date);
    else if (tree.status == 2)
        sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y, tree.species, date);
    else
        sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x, coordinates.y, tree.species, date);

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