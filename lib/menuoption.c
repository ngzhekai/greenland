#include "menuoption.h"
#include <stdio.h>
#include <stdlib.h>

char *moption_handle(MenuOption mo)
{
  char *msg = malloc(1024);

  switch (mo)
  {
  case PLANT_TREE:
  case QUERY_TREE:
  case UPDATE_TREE:
    sprintf(msg, "Option %d soon to be supported\n", mo);
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
  scanf("%d", (int *)opt);
  system("clear");
}
