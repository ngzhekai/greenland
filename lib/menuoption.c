#include "menuoption.h"
#include <stdio.h>

int moption_handle(MenuOption mo)
{
  switch (mo) {
    case PLANT_TREE:
    case QUERY_TREE:
    case UPDATE_TREE:
      printf("Option %d soon to be supported\n", mo);
      return 0;

    default:
      return -1;
  }
}
