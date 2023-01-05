#define _XOPEN_SOURCE // include support for strptime(3)
#include "tree.h"
#include "tree_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

Tree* tree_create(const char* sp, tree_state st, const char* d)
{
  Tree* new_tree = malloc(sizeof(Tree));
  tree_set_species(new_tree, sp);
  tree_set_status(new_tree, st);
  new_tree->day_planted = malloc(sizeof(struct tm));
  tree_set_day_planted(new_tree, d);

  return new_tree;
}

void tree_free(Tree* t)
{
  free(t->day_planted);
  free(t->species);
  free(t);
}

const char* tree_get_species(const Tree* t)
{
  return t->species;
}

tree_state tree_get_status(const Tree* t)
{
  return t->status;
}

struct tm* tree_get_day_planted(const Tree* t)
{
  return t->day_planted;
}

void tree_set_species(Tree* t, const char* sp)
{
  if ((t->str_size = strlen(sp) + 1) > 0) {
    t->species = calloc(t->str_size, sizeof(char));
    strncpy(t->species, sp, t->str_size);
  } else {
    t->str_size = 0;
    t->species = NULL;
  }
}

void tree_set_status(Tree* t, tree_state st)
{
  t->status = st;
}

void tree_set_day_planted(Tree* t, const char* d)
{
  if (tree_get_status(t) == DEAD) {
    t->day_planted = NULL;
  } else {
    strptime(d, "%Y-%m-%d", t->day_planted);
  }
}
