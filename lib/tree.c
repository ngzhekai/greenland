#include "tree.h"
#include <stdlib.h>
#include <string.h>

struct Tree* tree_create(const char* sp, uint_least8_t st, unsigned long d)
{
  struct Tree* new_tree = malloc(sizeof(struct Tree));
  new_tree->species = calloc(strlen(sp) + 1, sizeof(char));
  strncpy(new_tree->species, sp, strlen(sp) + 1);
  new_tree->status = st;
  new_tree->days_alived = d;

  return new_tree;
}

void tree_free(struct Tree *t)
{
  free(t->species);
  free(t);
}

char* tree_get_species(const struct Tree* t)
{
  return t->species;
}

uint_least8_t tree_get_status (const struct Tree* t)
{
  return t->status;
}

unsigned long tree_get_days_alived(const struct Tree* t)
{
  return t->days_alived;
}
