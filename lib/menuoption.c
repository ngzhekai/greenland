#include "../src/inet.h"
#include "../src/sem.h"
#include "menuoption.h"
#include "tree.h"
#include "tree_coordinate.h"
#include "tree_state.h"

/* semaphore function declaration */
int initsem(key_t semkey);
int p(int semid);
int v(int semid);

/* function declaration */
tree_coordinate get_coordinates(int sockfd, char* buffer);
int check_tree_exist(const char* filename, Tree* tree, int* treeIndex,
                     tree_coordinate* c, tree_coordinate* store);
void update_detail(const char* filename, Tree tree, int sockfd, char* buffer,
                   tree_coordinate store);
void copy_tree(const char* filename, tree_coordinate store);
void plant_tree_server(const char* filename, Tree tree, int new_sockfd,
                       char* buffer);
void update_tree_server(const char* filename, Tree tree, int new_sockfd,
                        char* buffer);
void query_tree_server(const char* filename, Tree tree, int new_sockfd,
                       char* buffer);

void sigchld_handler(int sig);

char* moption_handle(int new_sockfd, int semid, char* cli_addr, MenuOption mo)
{
  char buffer[BUFFER_SIZE];
  char* msg = malloc(1024);
  Tree tree;

  switch (mo) {
    case PLANT_TREE:
      p(semid); // lock the semaphore for writing
      plant_tree_server(FILENAME, tree, new_sockfd, buffer);
      p(semid); // lock the semaphore for writing
      sprintf(msg, "Client [%s]: Plant tree process done.", cli_addr);
      return msg;
      break;

    case QUERY_TREE:
      query_tree_server(FILENAME, tree, new_sockfd, buffer);
      sprintf(msg, "Client [%s]: Query tree process done.", cli_addr);
      return msg;
      break;

    case UPDATE_TREE:
      p(semid); // lock the semaphore for writing
      /* critical section */
      /* invoke update_tree_server function */
      update_tree_server(FILENAME, tree, new_sockfd, buffer);
      v(semid); // unlock the semaphore after writing
      sprintf(msg, "Client [%s]: Update tree process done.", cli_addr);
      return msg;
      break;

    case EXIT_PROGRAM:
      sprintf(msg, "Client [%s]: Exited program.", cli_addr);
      return msg;
      break;

    default:
      sprintf(msg, "Option %d not supported\n", mo);
      return msg;
  }
}

void moption_display(MenuOption* opt)
{
  printf("  ________                               .__                       .___ \n");
  printf(" /  _____/_______   ____   ____    ____  |  |  _____     ____    __| _/ \n");
  printf("/   \\  ___\\_  __ \\_/ __ \\_/ __ \\  /    \\ |  |  \\__  \\   /    \\  / __ |  \n");
  printf("\\    \\_\\  \\|  | \\/\\  ___/\\  ___/ |   |  \\|  |__ / __ \\_|   |  \\/ /_/ |  \n");
  printf(" \\______  /|__|    \\___  >\\___  >|___|  /|____/(____  /|___|  /\\____ |  \n");
  printf("        \\/             \\/     \\/      \\/            \\/      \\/      \\/  \n\n");

  printf("(implemented) Welcome to greenland, a place to plant tree!\n\n");
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
  scanf("%d", (int*)opt);
  system("clear");
}

const char* getMenuOptionName(enum MenuOption op)
{
  switch (op) {
    case PLANT_TREE:
      return "Plant Tree";

    case QUERY_TREE:
      return "Query Tree";

    case UPDATE_TREE:
      return "Update Tree";

    case EXIT_PROGRAM:
      return "Exit Program";

    default:
      return (char*) 0;
  }

}

void sigchld_handler(int sig)
{
  wait((int*)0);
}

tree_coordinate get_coordinates(int sockfd, char* buffer)
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

int check_tree_exist(const char* filename, Tree* tree, int* treeIndex,
                     tree_coordinate* c, tree_coordinate* store)
{
  FILE* fp;

  // variable INIT
  int numTrees = 0;
  char temp[BUFFER_SIZE];

  // Open file for reading
  if ((fp = fopen(filename, "r")) == NULL) {
    printf("Error opening the file %s for reading\n", filename);
    exit(0);
  }

  // Read tree data from the file and store in the trees array
  while (fgets(temp, 512, fp) != NULL) {
    int vertical, horizontal;
    char species[BUFFER_SIZE], status[BUFFER_SIZE], date[BUFFER_SIZE];
    sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species,
           date, status);
    printf("x: %d, y: %d\n", vertical,
           horizontal); // debug
    printf("compare x: %d, compare y: %d\n", tree_get_x(c),
           tree_get_y(c)); // debug

    store->x = vertical;
    store->y = horizontal;
    tree_set_species(tree,
                     species); // because species (sp) is a constant in the function so it will need to use tree_set_species()
    tree->status = atoi(status);
    tree->day_planted =
      NULL; // initiaties the day_planted (always makes sure it is initiated)
    tree_set_day_planted(tree,
                         date); //<---code got stuck here (fixed with initalization)

    printf("Debug: X coordinates print from Tree (store): %d\n",
           store->x); // debug
    printf("Debug: Y coordinates print from Tree (store): %d\n",
           store->y); // deubg

    if (vertical == tree_get_x(c) && horizontal == tree_get_y(c)) {
      *treeIndex = numTrees;
      // treeIndex can be used to show which line is the search tree located in the textfile;
      printf("Debug: The coordinates found was located on line %d\n",
             *treeIndex + 1);
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

void update_detail(const char* filename, Tree tree, int sockfd, char* buffer,
                   tree_coordinate store)
{
  // variable INIT
  int state = 0;
  int speciesChosen = 0;
  char sp[BUFFER_SIZE];
  tree_state st;

  printf("\nDebug Checkpoint: beginning of update_detail function (recv from client)\n");

  // get species from client
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  speciesChosen = atoi(buffer);

  if (speciesChosen == 1) {
    // strncpy(trees[*treeIndex].species, "Deciduous", 10);
    tree_set_species(&tree, "Deciduous");
    strcpy(sp, "Deciduous");
  } else {
    // strncpy(trees[*treeIndex].species, "Coniferous", 11);
    tree_set_species(&tree, "Coniferous");
    strcpy(sp, "Coniferous");
  }

  printf("Species selected\n"); // debug

  // get status from client
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  state = atoi(buffer);

  if (state == 0) {
    st = DEAD;
  } else if (state == 1) {
    st = ALIVE;
  } else if (state == 2) {
    st = SICK;
  } else {
    st = TREAMENT;
  }

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

  FILE* of;
  of = fopen(filename, "a");

  if (of == NULL) {
    printf("Error opening the file %s\n", filename);
    exit(0);
  }

  printf("%d\n", store.x);
  printf("%d\n", store.y);
  printf("%s\n", tree.species);
  printf("%s\n", time);
  printf("%d\n", tree.status);
  fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%d\n", store.x, store.y, tree.species, time,
          tree.status);

  fclose(of);

  // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", store.x, store.y, tree.species, time, tree.status); //discarded to hardcode the status text
  // send back to the client
  if (tree.status == 0) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", store.x, store.y, tree.species,
            time);
  } else if (tree.status == 1) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", store.x, store.y, tree.species,
            time);
  } else if (tree.status == 2) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", store.x, store.y, tree.species,
            time);
  } else {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", store.x, store.y,
            tree.species, time);
  }

  send(sockfd, buffer, BUFFER_SIZE, 0);

  printf("\n<-- debug Checkpoint! (writing completed from update_details() func) -->\n");
} /* end of update_detail() function */

void copy_tree(const char* filename, tree_coordinate store)
{
  FILE* fp;

  // Open file for reading
  if ((fp = fopen(filename, "r")) == NULL) {
    printf("Error opening the file %s for reading\n", filename);
    exit(0);
  }

  // variable INIT
  int numTrees = 0;
  char temp[BUFFER_SIZE];

  // Open file for copying into temporary text file
  FILE* tmp;
  tmp = fopen("temp.txt", "a");

  if (tmp == NULL) {
    printf("Error opening the file %s for temporary writing\n", filename);
    exit(0);
  }

  // Read tree data from the file and store in the trees array
  while (fgets(temp, 512, fp) != NULL) {
    int vertical, horizontal;
    char species[BUFFER_SIZE], date[BUFFER_SIZE], status[BUFFER_SIZE];
    sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species,
           date, status);

    if (!(vertical == store.x && horizontal == store.y)) {
      fprintf(tmp, "%d,%d\t\t%s\t\t%s\t\t%s\n", vertical, horizontal, species, date,
              status);
    }

    numTrees++;
  }

  fclose(tmp);
  fclose(fp);

  // rename temp.txt to test.txt
  if (rename("temp.txt", filename) == 0) {
    // printf("Copied successfully\n"); // debug purpose only
    unlink("temp.txt");
  } else {
    perror("Error: has occurred\n");
    exit(1);
  }
} /* end of copy_tree() function */

void plant_tree_server(const char* filename, Tree tree, int new_sockfd,
                       char* buffer)
{

  tree_coordinate coordinates;
  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate store;

  do {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);

    if (find_result) {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (find_result);

  /* critical section */
  update_detail(filename, tree, new_sockfd, buffer, store);
  /* end of critical section */

  // printf("This is the newly created tree details:\n\n");
  // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client end)
  // printf("-----------\t-------\t\t----\t\t------\n");

} /* end of plant_tree_server() function */

void update_tree_server(const char* filename, Tree tree, int new_sockfd,
                        char* buffer)
{

  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate coordinates;
  tree_coordinate store;

  do {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);

    if (find_result) {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (!find_result);

  // printf("This is the current tree details:\n\n");
  // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client ends)
  // printf("-----------\t-------\t\t----\t\t------\n");
  char date[BUFFER_SIZE];
  strftime(date, BUFFER_SIZE, "%Y-%m-%d",
           tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti

  // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", coordinates.x, coordinates.y, tree.species, date, tree.status);
  if (tree.status == 0) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else if (tree.status == 1) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else if (tree.status == 2) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x,
            coordinates.y, tree.species, date);
  }

  send(new_sockfd, buffer, BUFFER_SIZE, 0);

  /* critical section */
  copy_tree(filename, store);
  update_detail(filename, tree, new_sockfd, buffer, store);
  /* end of critical section */

  // printf("This is the updated tree details:\n\n");
  // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n");  // for debug (this is what it is going to show on the client end)
  // printf("-----------\t-------\t\t----\t\t------\n");

} /* end of update_tree_server() function */

void query_tree_server(const char* filename, Tree tree, int new_sockfd,
                       char* buffer)
{
  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate coordinates;
  tree_coordinate store;

  do {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);
    printf("findresult: %d", find_result);

    if (find_result) {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (!find_result);

  // printf("This is the query result:\n\n");
  // printf("Coordinates\tSpecies\t\tDate\t\tStatus\n"); // for debug (this is what it is going to show on the client end)
  // printf("-----------\t-------\t\t----\t\t------\n");
  // send to client
  char date[BUFFER_SIZE];
  strftime(date, BUFFER_SIZE, "%Y-%m-%d",
           tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti

  // sprintf(buffer, "%d,%d\t\t%s\t%s\t%d\n", coordinates.x, coordinates.y, tree.species, date, tree.status);
  if (tree.status == 0) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else if (tree.status == 1) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else if (tree.status == 2) {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y,
            tree.species, date);
  } else {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x,
            coordinates.y, tree.species, date);
  }

  send(new_sockfd, buffer, BUFFER_SIZE, 0);
} /* end of query_tree_server() function */

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

int p(int semid)
{
  struct sembuf p_buf;
  p_buf.sem_num = 0; /* semaphore number of the sepamore set,
                         value '0' shows the first semaphore in the set is chosen */

  p_buf.sem_op =
    -1; /* the semaphore value is decremented by the absolute value of sem_op (-1)
                           showing the locking operation of semaphore is done */

  p_buf.sem_flg =
    SEM_UNDO; /* SEM_UNDO operation flag is used to tell the system to undo the process's semaphore changes automaticall, when the process exits.
                                  This allows processes to avoid deadlock problems. */

  if (semop(semid, &p_buf, 1) == -1) {
    /* add explanation here */
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

  v_buf.sem_op =
    1; /* the semaphore value is incremented by the absolute value of sem_op (1),
                          showing the unlocking operation of semaphore is done. */

  v_buf.sem_flg =
    SEM_UNDO; /* SEM_UNDO operation flag is used to tell the system to undo the process's semaphore changes automaticall, when the process exits.
                                  This allows processes to avoid deadlock problems. */

  if (semop(semid, &v_buf, 1) == -1) {
    /* perform the semaphore operation as specified in v_buf struct on semaphore with the id (semid)
      the last argument '1' shows the number of sembuf structure in the array. */
    perror("V (semid) fails");
    exit(1);
  }

  return (0);
}