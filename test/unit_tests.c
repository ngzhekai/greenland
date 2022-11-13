#include "../lib/tree.h"
#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

START_TEST(test_tree_create)
{
  struct Tree* t;

  t = tree_create("foo", 0, 1);

  ck_assert_str_eq(tree_get_species(t), "foo");
  ck_assert(tree_get_status(t) == (uint_least8_t) 0);
  ck_assert(tree_get_days_alived(t) == (unsigned long) 1);

  tree_free(t);
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

  tcase_add_test(tc_core, test_tree_create);
  suite_add_tcase(s, tc_core);

  return s;
}
