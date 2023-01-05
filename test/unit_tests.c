#define _XOPEN_SOURCE // include support for strptime(3)
#include "../lib/tree.h"
#include "../lib/menuoption.h"
#include "../lib/species.h"
#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

START_TEST(test_tree_create_dead)
{
  struct Tree* t;

  t = tree_create("foo", DEAD, "2022-12-05");

  ck_assert_uint_eq(t->str_size, 4);
  ck_assert_str_eq(tree_get_species(t), "foo");
  ck_assert(tree_get_status(t) == DEAD);
  ck_assert(t->day_planted == NULL);

  tree_free(t);
}
END_TEST

START_TEST(test_tree_create_normal)
{
  struct Tree* t;

  t = tree_create("foo", PLANTED, "2022-12-05");

  char* d = malloc(sizeof(char) * 11);
  strftime(d, sizeof(char) * 11, "%F", tree_get_day_planted(t));

  ck_assert_uint_eq(t->str_size, 4);
  ck_assert_str_eq(tree_get_species(t), "foo");
  ck_assert(tree_get_status(t) == PLANTED);
  ck_assert_str_eq(d, "2022-12-05");

  tree_free(t);
}
END_TEST

START_TEST(test_trstate_to_string)
{
  ck_assert_str_eq(trstat_to_string(DEAD), "DEAD");
  ck_assert_str_eq(trstat_to_string(PLANTED), "PLANTED");
  ck_assert_str_eq(trstat_to_string(SICK), "SICK");
  ck_assert_str_eq(trstat_to_string(TREAMENT), "TREAMENT");
  ck_assert_str_eq(trstat_to_string(-1), "UNKNOWN STATE");
}
END_TEST

START_TEST(test_trstate_is_valid)
{
  ck_assert(trstat_is_valid(DEAD) == true);
  ck_assert(trstat_is_valid(-1) == false);
}
END_TEST

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
START_TEST(test_species_is_valid)
{
  char* list_of_species[] = {
    "Durio zibethinus",
    "Musa acuminata",
    "Acacia mangium",
    "Tectona grandis",
  };

  ck_assert(species_is_valid("Acacia mangium", list_of_species) == true);
  ck_assert(species_is_valid("Ingram ham", list_of_species) == false);
}
END_TEST
#pragma GCC diagnostic pop

START_TEST(test_menuoption_handle)
{
  // ck_assert_str_eq(moption_handle(PLANT_TREE), 0);
  // ck_assert_str_eq(moption_handle(QUERY_TREE), 0);
  // ck_assert_str_eq(moption_handle(UPDATE_TREE), 0);
  ck_assert_str_eq(moption_handle(-1), "Option -1 not supported\n");
}
END_TEST

Suite* tree_suit(void);
Suite* menu_suit(void);

int main(void)
{
  int num_failed;
  Suite* s;
  SRunner* sr;

  s = tree_suit();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  num_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

Suite* tree_suit(void)
{
  Suite* s;
  TCase* tc_core;

  s = suite_create("Tree");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_tree_create_dead);
  tcase_add_test(tc_core, test_tree_create_normal);
  tcase_add_test(tc_core, test_trstate_to_string);
  tcase_add_test(tc_core, test_trstate_is_valid);
  tcase_add_test(tc_core, test_species_is_valid);
  suite_add_tcase(s, tc_core);

  return s;
}

Suite* menu_suit(void)
{
  Suite* s;
  TCase* tc_core;

  s = suite_create("Menu");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_menuoption_handle);
  suite_add_tcase(s, tc_core);

  return s;
}
