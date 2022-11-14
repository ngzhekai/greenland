#include "../lib/tree.h"
#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

START_TEST(test_tree_create_dead)
{
  struct Tree* t;

  t = tree_create("foo", DEAD, 1);

  ck_assert_str_eq(tree_get_species(t), "foo");
  ck_assert(tree_get_status(t) == DEAD);
  ck_assert(tree_get_days_alived(t) == (unsigned long) 0);

  tree_free(t);
}
END_TEST

START_TEST(test_tree_create_normal)
{
  struct Tree* t;

  t = tree_create("foo", PLANTED, 10);

  ck_assert_str_eq(tree_get_species(t), "foo");
  ck_assert(tree_get_status(t) == PLANTED);
  ck_assert(tree_get_days_alived(t) == (unsigned long) 10);

  tree_free(t);
}
END_TEST

START_TEST(test_trstate_to_string)
{
  ck_assert_str_eq(trstat_to_string(DEAD), "DEAD");
  ck_assert_str_eq(trstat_to_string(PLANTED), "PLANTED");
  ck_assert_str_eq(trstat_to_string(SICK), "SICK");
  ck_assert_str_eq(trstat_to_string(TREAMENT), "TREAMENT");
  ck_assert(trstat_to_string(-1) == (char*) 0);
}
END_TEST

START_TEST(test_trstate_is_valid)
{
  ck_assert(trstat_is_valid(DEAD) == true);
  ck_assert(trstat_is_valid(-1) == false);
}
END_TEST

Suite* tree_suit(void);

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
  suite_add_tcase(s, tc_core);

  return s;
}
