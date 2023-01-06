#include "tree_coordinate.h"

int tree_get_x(const tree_coordinate* c)
{
  return c->x;
}

int tree_get_y(const tree_coordinate* c)
{
  return c->y;
}

void tree_set_x(tree_coordinate* c, int x)
{
  c->x = x;
}

void tree_set_y(tree_coordinate* c, int y)
{
  c->y = y;
}