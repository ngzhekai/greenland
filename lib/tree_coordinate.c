#include "tree_coordinate.h"

// bool trcdn_is_valid(tree_coordinate c)
// {
//     // Check if coordinate is occupied
//     //   if () {
//     //     return false;
//     //   }
//     return true;
// }

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