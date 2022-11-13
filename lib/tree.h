/**
 * @file
 * @date 2022-11-11 23:27:54 PM +0800
 */
#ifndef TREE_H
#define TREE_H

#include <stdint.h>

/**
 * Tree data class that stores information about a tree including its species,
 * its status, and the amount of days it has lived.
 */
struct Tree {
  /** Species name of the tree */
  char* species;
  /** Status of the tree */
  uint_least8_t status;
  /** Amount of days the tree has lived */
  unsigned long days_alived;
};

/**
 * Create a Tree data class
 *
 * @param[in] sp Species name for the Tree
 * @param[in] st Current status for the Tree
 * @param[in] d Days alived for the Tree
 *
 * @return A Tree data class that contains the information about its species
 * name, current status and the amount of days alived.
 */
struct Tree* tree_create(const char* sp, uint_least8_t st, unsigned long d);

/**
 * Destroy a Tree data class
 *
 * @param[in] t A tree data class to be destroyed
 */
void tree_free(struct Tree* t);

/**
 * Get Tree's species name
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A string that describes the Tree's species name
 */
char* tree_get_species(const struct Tree* t);

/**
 * Get Tree's current status
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A non-negative number indicates the Tree's current status
 */
uint_least8_t tree_get_status(const struct Tree* t);

/**
 * Get Tree's the amount of days it has lived
 *
 * @param[in] t A pointer to a Tree
 *
 * @return A non-negative number of days the Tree has lived
 */
unsigned long tree_get_days_alived(const struct Tree* t);

#endif
