/**
 * @file
 * @date 2022-11-16 23:05:32 PM +0800
 */
#ifndef MENUOPTION_H
#define MENUOPTION_H

#include "tree.h"

/**
 * MenuOption defines available options in the client menu
 */
typedef enum MenuOption { PLANT_TREE = 1, QUERY_TREE, UPDATE_TREE } MenuOption;

/**
 * Handles menu option
 *
 * @param[in] mo A MenuOption enum to be handle
 * @param[out] t A pointer to tree structure, could be uninitialised
 *
 * @return 0 if the handling succeeded, -1 if failed with ENOSUP as errno
 */
int moption_handle(MenuOption mo, Tree* t);

/**
 * Display menu options
 *
 * @param[out] opt A MenuOption got from user's input
 */
void moption_display(MenuOption* opt);

/**
 * Display Plant Tree menu and prompt user for tree's information
 *
 * @return A Tree initialised with prompted user values
 */
Tree* moption_plant_tree(void);

#endif
