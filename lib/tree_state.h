/**
 * @file
 * @date 2022-11-14 10:32:23 AM +0800
 */
#ifndef TREE_STATE_H
#define TREE_STATE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * An enum define the state of a Tree
 */
typedef enum tree_state { DEAD, PLANTED, SICK, TREAMENT } tree_state;

/**
 * Convert tree state into readable string
 *
 * @param[in] st A tree state
 *
 * @return A string that specifies the state of the Tree, NULL if the state is
 *         unknown
 */
const char* trstat_to_string(tree_state st);

/**
 * Test whether the tree state is valid
 *
 * @param[in] st a tree state
 *
 * @return true the tree state is valid, false if it is not
 */
bool trstat_is_valid(tree_state st);

#endif