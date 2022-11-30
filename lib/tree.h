/**
 * @file
 * @date 2022-11-11 23:27:54 PM +0800
 */
#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <time.h>
#include "tree_state.h"

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
} Tree;

/**
 * Create a Tree data class
 *
 * @param[in] sp A string specifies the species name for the Tree
 * @param[in] st A Tree State enum specifies the current status for the
 *               Tree
 * @param[in] d A time_t in seconds specifies the days alived of the Tree
 *
 * @return A Tree data class that contains the information about its species
 * name, current status and the amount of days alived.
 *
 * @attention st must be valid
 */
struct Tree* tree_create(const char* sp, tree_state st, time_t d);

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
 * Set Tree's the amount of days it has lived
 *
 * @param[out] t A modified Tree with changed days of lived
 * @param[in] d A time_t in seconds
 */
void tree_set_day_planted(Tree* t, time_t d);

#endif
