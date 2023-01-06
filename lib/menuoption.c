#include "menuoption.h"
#include "date.h"
#include "tree.h"
#include "tree_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#define BUFSIZE 512

int moption_handle(MenuOption mo, Tree* t)
{
  Tree* nt = (Tree*) 0;

  switch (mo) {
    case PLANT_TREE:
      nt = moption_plant_tree();
      t = realloc(t, sizeof(*nt));
      bcopy(nt, t, sizeof(*nt));
      break;

    case QUERY_TREE:
    case UPDATE_TREE:
      printf("Option %d soon to be supported\n", mo);
      break;

    default:
      errno = ENOTSUP;
      printf("Option %d not supported\n", mo);
      return -1;
  }

  free(nt);
  return 0;
}

void moption_display(MenuOption* opt)
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
  printf("(Choose a service needed according to the number)\n\n");
  printf("Option: ");
  scanf("%d", (int*) opt);
  system("clear");
}

Tree* moption_plant_tree(void)
{
  printf("################################\n");
  printf("#                              #\n");
  printf("#        PLANTING TREE         #\n");
  printf("#                              #\n");
  printf("################################\n\n");
  printf("Press <Enter> to agree to default value\n\n");

  char species[BUFSIZE], day_planted[BUFSIZE];
  char status[BUFSIZE] = "-1";
  int nbytes;

  write(STDOUT_FILENO, "Tree's species: ", 17);

  if ((nbytes = read(STDIN_FILENO, species, BUFSIZE)) < 0) {
    perror("[-] Input error!");
    exit(1);
  }

  species[nbytes - 1] = '\0';

  getchar();  // ignore new line

  while (!trstat_is_valid(atoi(status))) {
    printf("Tree's status: (default PLANTED) ");
    fgets(status, BUFSIZE, stdin);

    if (status[0] == '\n') {
      bzero(status, BUFSIZE);
      strcpy(status, "1");
    }

    status[strcspn(status, "\n")] = 0;  // remove newline

    if (!trstat_is_valid(atoi(status))) {
      printf("[-] Input error!: Status must be within range of 0 and 3\n");
    }
  }

  while (true) {
    printf("Day when the tree is planted: (default local time, in format YYYY-MM-DD) ");
    fgets(day_planted, BUFSIZE, stdin);

    if (day_planted[0] == '\n') {
      time_t current_time = time(NULL);
      strftime(day_planted, BUFSIZE, "%F", localtime(&current_time));
    }

    day_planted[strcspn(day_planted, "\n")] = 0;  // remove newline

    if (!date_is_valid(day_planted)) {
      printf("[-] Input error!: Date must be valid and in format of YYYY-MM-DD\n");
    } else {
      break;
    }
  }

  return tree_create(species, atoi(status), day_planted);
}
