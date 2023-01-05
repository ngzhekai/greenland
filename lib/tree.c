#define _XOPEN_SOURCE // include support for strptime(3)
#include "tree.h"
#include "tree_state.h"
#include "tree_coordinate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

Tree *tree_create(const char *sp, tree_state st, const char *d, tree_coordinate c)
{
  Tree *new_tree = malloc(sizeof(Tree));
  tree_set_species(new_tree, sp);
  tree_set_status(new_tree, st);
  new_tree->day_planted = NULL;
  tree_set_day_planted(new_tree, d);
  tree_set_coordinate(new_tree, c);
  return new_tree;
}

// test this using another branch
// void tree_serialise(Tree* t, char* buf)
// {
//   bcopy(&(t->str_size), buf, sizeof(unsigned long));
//   bcopy(t->species, buf + sizeof(unsigned long), t->str_size);
//   bcopy(&(t->status), buf + t->str_size, sizeof(tree_state));
//   bcopy(t->day_planted, buf + sizeof(tree_state), sizeof(struct tm));
// }

// Tree* tree_deserialise(char* s)
// {
//   unsigned long str_size;
//   bcopy(s, &str_size, sizeof(unsigned long));
//   printf("str size: %lu\n", str_size);

//   char* species = calloc(str_size + 1, sizeof(char));
//   bcopy(s + sizeof(unsigned long), species, str_size);
//   printf("species: %s\n", species);

//   tree_state status;
//   bcopy(s + str_size, &status, sizeof(tree_state));
//   printf("status: %s\n", trstat_to_string(status));

//   struct tm day_planted;
//   bcopy(s + sizeof(tree_state), &day_planted, sizeof(struct tm));

//   char* date_s = malloc(11);
//   strptime(date_s, "%F", &day_planted);
//   printf("date: %s\n", date_s);

//   Tree* t = tree_create(species, status, date_s);
//   printf("New tree: species %s: status: %s\n", t->species,
//          trstat_to_string(status));

//   return t;
// }

void tree_free(Tree *t)
{
  free(t->day_planted);
  free(t->species);
  free(t);
}

char *tree_get_species(const Tree *t)
{
  return t->species;
}

tree_state tree_get_status(const Tree *t)
{
  return t->status;
}

struct tm *tree_get_day_planted(const Tree *t)
{
  return t->day_planted;
}

tree_coordinate tree_get_coordinate(const Tree *t)
{
  return t->coordinate;
}

void tree_set_species(Tree *t, const char *sp)
{
  if ((t->str_size = strlen(sp) + 1) > 0)
  {
    t->species = calloc(t->str_size, sizeof(char));
    strncpy(t->species, sp, t->str_size);
  }
  else
  {
    t->str_size = 0;
    t->species = NULL;
  }
}

void tree_set_status(Tree *t, tree_state st)
{
  t->status = st;
}

void tree_set_day_planted(Tree *t, const char *d)
{
  if (tree_get_status(t) == DEAD)
  {
    printf("debug: it is dead\n");
    t->day_planted = NULL;
  }
  else
  {
    printf("debug: it is alive\n");
    if (!t->day_planted)
    { // handle the case when day_planted haven't allocated with memory
      printf("debug: day_planted is null\n");
      t->day_planted = malloc(sizeof(struct tm));
    }
    printf("debug: date: %s\n", d);
    strptime(d, "%F", t->day_planted);
  }
}

void tree_set_coordinate(Tree *t, tree_coordinate c)
{
  t->coordinate = c;
}
