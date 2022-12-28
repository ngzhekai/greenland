#include <stdint.h>
#include <stdbool.h>

typedef struct tree_coordinate
{
    int x;
    int y;
} tree_coordinate;

bool trloc_is_valid(tree_coordinate c);
