#include "inet.h"
#include "menuoption.h"
#include "plant_tree.h"

char *moption_handle(int* client_socket, MenuOption mo) // called by server
{
  char *msg = malloc(1024);
  char buffer[1024];

  switch (mo)
  {
  case PLANT_TREE:
    bzero(buffer, 1024);
    char *sp = malloc(1024);
    tree_state st = PLANTED;
    char *d = malloc(1024);
    tree_coordinate c;

    recv(*client_socket, buffer, sizeof(buffer), 0);
    strcpy(sp, buffer);

    recv(*client_socket, buffer, sizeof(buffer), 0);    
    strcpy(d, buffer);

    recv(*client_socket, buffer, sizeof(buffer), 0);
    c.x = atoi(buffer);

    recv(*client_socket, buffer, sizeof(buffer), 0);
    c.y = atoi(buffer);

    tree_create(sp, st, d, c);
    sprintf(msg, "\nTree successfully planted.");
    return msg;

  case QUERY_TREE:
  case UPDATE_TREE:
    sprintf(msg, "Option %d soon to be supported\n", mo);
    return msg;

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
