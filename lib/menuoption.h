/**
 * @file
 * @date 2022-11-16 23:05:32 PM +0800
 */
#ifndef MENUOPTION_H
#define MENUOPTION_H

/**
 * MenuOption defines available options in the client menu
 */
typedef enum MenuOption { PLANT_TREE = 1, QUERY_TREE, UPDATE_TREE } MenuOption;

/**
 * Handles menu option
 *
 * @param[in] client_socket Descriptor of client's socket
 * 
 * @param[in] mo A MenuOption enum to be handle
 *
 * @return 0 if the handling succeeded, -1 if failed
 */
char* moption_handle(int client_socket, MenuOption mo);

/**
 * Display menu options
 *
 * @param[out] opt A MenuOption got from user's input
 */
void moption_display(MenuOption* opt);

#endif
