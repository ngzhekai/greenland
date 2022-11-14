#include "tree.h"
#include "tree_state.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct Tree* tree_create(const char* sp, tree_state st, unsigned long d)
{
  struct Tree* new_tree = malloc(sizeof(struct Tree));
  assert(trstat_is_valid(st));
  new_tree->species = NULL;
  tree_set_species(new_tree, sp);
  tree_set_status(new_tree, st);
  tree_set_days_alived(new_tree, d);

  return new_tree;
}

void tree_free(struct Tree* t)
{
  free(t->species);
  free(t);
}

char* tree_get_species(const struct Tree* t)
{
  return t->species;
}

tree_state tree_get_status(const struct Tree* t)
{
  return t->status;
}

unsigned long tree_get_days_alived(const struct Tree* t)
{
  return t->days_alived;
}

void tree_set_species(struct Tree* t, const char* sp)
{
  if (t->species) {
    free(t->species);
  }

  t->species = calloc(strlen(sp) + 1, sizeof(char));
  strncpy(t->species, sp, strlen(sp) + 1);
}

void tree_set_status(struct Tree* t, tree_state st)
{
  t->status = st;
}

void tree_set_days_alived(struct Tree* t, unsigned long d)
{
  if (tree_get_status(t) == DEAD) {
    t->days_alived = 0;
  } else {
    t->days_alived = d;
  }
}
