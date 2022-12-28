/**
 * @file
 * @date 2022-11-11 23:27:54 PM +0800
 */
#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <time.h>
#include "tree_state.h"
#include "tree_coordinate.h"

/**
 * Tree data class that stores information about a tree including its species,
 * its status, and the amount of days it has lived.
 */
typedef struct Tree {
  /** Species name of the tree */
  char* species;
  /** Status of the tree */
  tree_state status;
  /** The day when the tree was planted */
  struct tm* day_planted;
  /** The coordinate where the tree was planted */
  tree_coordinate coordinate;
} Tree;

/**
 * Create a Tree data class
 *
 * @param[in] sp A string specifies the species name for the Tree
 * @param[in] st A Tree State enum specifies the current status for the
 *               Tree
 * @param[in] d A string specifies the days alived of the Tree in format
 *              %Y-%m-%d, e.g. 2022-12-05
 * 
 * @param[in] c A Tree coordinate struct that specify the longitude and latitude
 *              coordinate of the planted tree
 *
 * @return A Tree data class that contains the information about its species
 * name, current status and the amount of days alived.
 *
 * @attention st must be valid
 */
struct Tree* tree_create(const char* sp, tree_state st, const char* d, tree_coordinate c);

/**
 * Destroy a Tree data class
 *
 * @param[in] t A tree data class to be destroyed
 */
void tree_free(Tree* t);

/**
 * Get Tree's species name
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A string that describes the Tree's species name
 */
char* tree_get_species(const Tree* t);

/**
 * Get Tree's current status
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A non-negative integer indicates the Tree's current status
 */
tree_state tree_get_status(const Tree* t);

/**
 * Get Tree's the amount of days it has lived
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A pointer to struct tm indicates the day when the Tree was planted
 */
struct tm* tree_get_day_planted(const Tree* t);

/**
 * Get Tree's the current coordinate
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A pointer to struct tree_coordinate indicates the coordinate where the Tree was planted
 */
tree_coordinate tree_get_coordinate(const Tree *t);

/**
 * Set Tree's species name
 *
 * @param[out] t A modified Tree with changed species name
 * @param[in] sp A string indicates a species name
 */
void tree_set_species(Tree* t, const char* sp);

/**
 * Set Tree's current status
 *
 * @param[out] t A modified Tree with changed status
 * @param[in] st A Tree State enum
 */
void tree_set_status(Tree* t, tree_state st);

/**
 * Set Tree's when it was planted
 *
 * @param[out] t A modified Tree with changed planted day
 * @param[in] d A string specifies the planted day
 */
void tree_set_day_planted(Tree* t, const char* d);


/**
 * Set Tree's coordinate where it was planted
 *
 * @param[out] t A modified Tree with changed planted coordinate
 * @param[in] d A struct specifies the coordinate
 */
void tree_set_coordinate(Tree *t, tree_coordinate c);

#endif
