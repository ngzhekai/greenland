/**
 * @file
 * @date 2022-11-14 10:32:23 AM +0800
 */
#ifndef TREE_COORDINATE_H
#define TREE_COORDINATE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct tree_coordinate
{
    int x;
    int y;
} tree_coordinate;

void tree_set_x(tree_coordinate *c, int x);

void tree_set_y(tree_coordinate *c, int y);

int tree_get_x(const tree_coordinate *c);

int tree_get_y(const tree_coordinate *c);

// bool trcdn_is_valid(tree_coordinate c);

#endif