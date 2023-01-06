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
tree_coordinate get_coordinates(int sockfd, char *buffer);
int check_tree_exist(const char *filename, Tree *tree, int *treeIndex,
                     tree_coordinate *c, tree_coordinate *store);
void update_detail(const char *filename, Tree tree, int sockfd, char *buffer,
                   tree_coordinate store);
void copy_tree(const char *filename, tree_coordinate store);
void plant_tree_server(const char *filename, Tree tree, int new_sockfd,
                       char *buffer);
void update_tree_server(const char *filename, Tree tree, int new_sockfd,
                        char *buffer);
void query_tree_server(const char *filename, Tree tree, int new_sockfd,
                       char *buffer);
void display_all_tree(const char *filename, int new_sockfd,
                      char *buffer);

void sigchld_handler(int sig);

char *moption_handle(int new_sockfd, int semid, char *cli_addr, MenuOption mo)
{
  char buffer[BUFFER_SIZE];
  char *msg = malloc(1024);
  Tree tree;

  switch (mo)
  {
  case PLANT_TREE:
    p(semid); // lock the semaphore for writing
    plant_tree_server(FILENAME, tree, new_sockfd, buffer);
    v(semid); // unlock the semaphore after writing
    sprintf(msg, "Client [%s]: Plant tree process done.", cli_addr);
    return msg;

  case QUERY_TREE:
    query_tree_server(FILENAME, tree, new_sockfd, buffer);
    sprintf(msg, "Client [%s]: Query tree process done.", cli_addr);
    return msg;

  case UPDATE_TREE:
    p(semid); // lock the semaphore for writing
    /* critical section */
    /* invoke update_tree_server function */
    update_tree_server(FILENAME, tree, new_sockfd, buffer);
    v(semid); // unlock the semaphore after writing
    sprintf(msg, "Client [%s]: Update tree process done.", cli_addr);
    return msg;

  case EXIT_PROGRAM:
    sprintf(msg, "Client [%s]: Exited program.", cli_addr);
    return msg;

  case DISPLAY_ALL_TREES:
    display_all_tree(FILENAME, new_sockfd, buffer);
    sprintf(msg, "Client [%s]: Display tree process done.", cli_addr);
    return msg;

  default:
    sprintf(msg, "Option %d not supported\n", mo);
    return msg;
  }
}

void moption_display(MenuOption *opt)
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
  printf("\t5. To display all the tree\n");
  printf("(Choose a service needed according to the number)\n\n");
  printf("Option: ");
  scanf("%d", (int *)opt);
  system("clear");
}

const char *getMenuOptionName(enum MenuOption op)
{
  switch (op)
  {
  case PLANT_TREE:
    return "Plant Tree";

  case QUERY_TREE:
    return "Query Tree";

  case UPDATE_TREE:
    return "Update Tree";

  case EXIT_PROGRAM:
    return "Exit Program";

  case DISPLAY_ALL_TREES:
    return "Display All Trees";

  default:
    return (char *)0;
  }
}

void sigchld_handler(int sig)
{
  wait((int *)0);
}

tree_coordinate get_coordinates(int sockfd, char *buffer)
{
  tree_coordinate c;
  int num;
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  num = atoi(buffer);
  tree_set_x(&c, num);
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  num = atoi(buffer);
  tree_set_y(&c, num);
  return c;
} /* end of get_coordinates() function */

int check_tree_exist(const char *filename, Tree *tree, int *treeIndex,
                     tree_coordinate *c, tree_coordinate *store)
{
  FILE *fp;

  // variable INIT
  int numTrees = 0;
  char temp[BUFFER_SIZE];

  // Open file for reading
  if ((fp = fopen(filename, "r")) == NULL)
  {
    printf("Error opening the file %s for reading\n", filename);
    exit(0);
  }

  // Read tree data from the file and store in the trees array
  while (fgets(temp, 512, fp) != NULL)
  {
    int vertical, horizontal;
    char species[BUFFER_SIZE], status[BUFFER_SIZE], date[BUFFER_SIZE];
    sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species,
           date, status);

    store->x = vertical;
    store->y = horizontal;
    tree_set_species(tree,
                     species); // because species (sp) is a constant in the function so it will need to use tree_set_species()
    tree->status = atoi(status);
    tree->day_planted =
        NULL; // initiaties the day_planted (always makes sure it is initiated)
    tree_set_day_planted(tree,
                         date); //<---code got stuck here (fixed with initalization)

    if (vertical == tree_get_x(c) && horizontal == tree_get_y(c))
    {
      *treeIndex = numTrees;
      return 1;
    }

    numTrees++;
  }

  tree_set_x(store, tree_get_x(c));
  tree_set_y(store, tree_get_y(c));
  fclose(fp);
  return 0;
} /* end of check_tree_exist() function */

void update_detail(const char *filename, Tree tree, int sockfd, char *buffer,
                   tree_coordinate c)
{
  // variable INIT
  int state = 0;
  int speciesChosen = 0;
  char sp[BUFFER_SIZE];
  tree_state st;
  printf("Coordinate set [X = %d, Y = %d]\n", c.x, c.y);

  // get species from client
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  speciesChosen = atoi(buffer);

  if (speciesChosen == 1)
  {
    tree_set_species(&tree, "Deciduous");
    strcpy(sp, "Deciduous");
  }
  else
  {
    tree_set_species(&tree, "Coniferous");
    strcpy(sp, "Coniferous");
  }

  printf("Species selected [%s]\n", sp);

  // get status from client
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  state = atoi(buffer);

  if (state == 0)
  {
    st = DEAD;
  }
  else if (state == 1)
  {
    st = ALIVE;
  }
  else if (state == 2)
  {
    st = SICK;
  }
  else
  {
    st = TREAMENT;
  }

  tree.status = st;
  printf("State selected [%s]\n", trstat_to_string(st));

  // get date from client
  recv(sockfd, buffer, BUFFER_SIZE, 0);
  char time[] = "2020-11-11";

  tree.day_planted = NULL;

  strncpy(time, buffer, BUFFER_SIZE);
  tree_set_day_planted(&tree, time);
  printf("Date set [%s]\n", time);

  FILE *of;
  of = fopen(filename, "a");

  if (of == NULL)
  {
    printf("Error opening the file %s\n", filename);
    exit(0);
  }

  fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%d\n", c.x, c.y, tree.species, time,
          tree.status);

  fclose(of);

  if (tree.status == 0)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", c.x, c.y, tree.species,
            time);
  }
  else if (tree.status == 1)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", c.x, c.y, tree.species,
            time);
  }
  else if (tree.status == 2)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", c.x, c.y, tree.species,
            time);
  }
  else
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", c.x, c.y,
            tree.species, time);
  }

  send(sockfd, buffer, BUFFER_SIZE, 0);
} /* end of update_detail() function */

void copy_tree(const char *filename, tree_coordinate store)
{
  FILE *fp;

  // Open file for reading
  if ((fp = fopen(filename, "r")) == NULL)
  {
    printf("Error opening the file %s for reading\n", filename);
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
    printf("Error opening the file %s for temporary writing\n", filename);
    exit(0);
  }

  // Read tree data from the file and store in the trees array
  while (fgets(temp, 512, fp) != NULL)
  {
    int vertical, horizontal;
    char species[BUFFER_SIZE], date[BUFFER_SIZE], status[BUFFER_SIZE];
    sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &vertical, &horizontal, species,
           date, status);

    if (!(vertical == store.x && horizontal == store.y))
    {
      fprintf(tmp, "%d,%d\t\t%s\t\t%s\t\t%s\n", vertical, horizontal, species, date,
              status);
    }

    numTrees++;
  }

  fclose(tmp);
  fclose(fp);

  // rename temp.txt to test.txt
  if (rename("temp.txt", filename) == 0)
  {
    unlink("temp.txt");
  }
  else
  {
    perror("Error: has occurred\n");
    exit(1);
  }
} /* end of copy_tree() function */

void plant_tree_server(const char *filename, Tree tree, int new_sockfd,
                       char *buffer)
{

  tree_coordinate coordinates;
  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate store;

  do
  {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);

    if (find_result)
    {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else
    {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (find_result);

  /* critical section */
  update_detail(filename, tree, new_sockfd, buffer, store);
  /* end of critical section */

} /* end of plant_tree_server() function */

void update_tree_server(const char *filename, Tree tree, int new_sockfd,
                        char *buffer)
{

  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate coordinates;
  tree_coordinate store;

  do
  {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);

    if (find_result)
    {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else
    {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (!find_result);

  char date[BUFFER_SIZE];
  strftime(date, BUFFER_SIZE, "%Y-%m-%d",
           tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti

  if (tree.status == 0)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else if (tree.status == 1)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else if (tree.status == 2)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x,
            coordinates.y, tree.species, date);
  }

  send(new_sockfd, buffer, BUFFER_SIZE, 0);

  /* critical section */
  copy_tree(filename, store);
  update_detail(filename, tree, new_sockfd, buffer, store);
  /* end of critical section */

} /* end of update_tree_server() function */

void query_tree_server(const char *filename, Tree tree, int new_sockfd,
                       char *buffer)
{
  int find_result = 0;
  int treeIndex = -1;
  tree_coordinate coordinates;
  tree_coordinate store;

  do
  {
    coordinates = get_coordinates(new_sockfd, buffer);
    find_result = check_tree_exist(filename, &tree, &treeIndex, &coordinates,
                                   &store);

    if (find_result)
    {
      send(new_sockfd, "1", BUFFER_SIZE, 0);
    }

    else
    {
      send(new_sockfd, "0", BUFFER_SIZE, 0);
    }

  } while (!find_result);

  // send to client
  char date[BUFFER_SIZE];
  strftime(date, BUFFER_SIZE, "%Y-%m-%d",
           tree.day_planted); // refer here: https://www.ibm.com/docs/en/i/7.3?topic=functions-strftime-convert-datetime-string#strfti

  if (tree.status == 0)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t0 (DEAD)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else if (tree.status == 1)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t1 (ALIVE)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else if (tree.status == 2)
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t2 (Sick)\n", coordinates.x, coordinates.y,
            tree.species, date);
  }
  else
  {
    sprintf(buffer, "%d,%d\t\t%s\t%s\t3 (Treatment)\n", coordinates.x,
            coordinates.y, tree.species, date);
  }

  send(new_sockfd, buffer, BUFFER_SIZE, 0);
} /* end of query_tree_server() function */

void display_all_tree(const char *filename, int new_sockfd,
                      char *buffer)
{
  printf("\n\nFunction invoked.\n\n");
  dup2(new_sockfd, 1);
  execl("/bin/cat", "cat", filename, (char *)0);
  
  // execl("/bin/wc", "wc", "-l", filename, (char *)0);
}

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

  if (semop(semid, &p_buf, 1) == -1)
  {
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

  if (semop(semid, &v_buf, 1) == -1)
  {
    /* perform the semaphore operation as specified in v_buf struct on semaphore with the id (semid)
      the last argument '1' shows the number of sembuf structure in the array. */
    perror("V (semid) fails");
    exit(1);
  }

  return (0);
}