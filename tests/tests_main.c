#include <stdio.h>

#define TESTS_MAIN
#include "tests.h"

DECLARE_TEST(option);
DECLARE_TEST(result_match);
DECLARE_TEST(result_inspect);
DECLARE_TEST(result_unwrap);
// DECLARE_TEST(result_and_then);
DECLARE_TEST(result_or_else);

DECLARE_TEST(option_match);
DECLARE_TEST(option_try);
DECLARE_TEST(option_unwrap);
DECLARE_TEST(option_unwrap_or);
DECLARE_TEST(option_take);
DECLARE_TEST(option_or_else);

DECLARE_TEST(arena_init);
DECLARE_TEST(arena_destroy);
DECLARE_TEST(arena);
DECLARE_TEST(arena_with_structs);

DECLARE_TEST(logger_init_fail);
DECLARE_TEST(logger_init_success);
DECLARE_TEST(default_fmt);

int main() {
  printf("tests\n");

  ADD_TEST(option_match);
  ADD_TEST(option_try);
  ADD_TEST(option_unwrap);
  ADD_TEST(option_unwrap_or);
  ADD_TEST(option_take);
  ADD_TEST(option_or_else);
  
  ADD_TEST(result_match);
  ADD_TEST(result_inspect);
  ADD_TEST(result_unwrap);
  // ADD_TEST(result_and_then);
  ADD_TEST(result_or_else);
  
  ADD_TEST(arena_init);
  ADD_TEST(arena_destroy);
  ADD_TEST(arena);
  ADD_TEST(arena_with_structs);

  ADD_TEST(logger_init_fail);
  ADD_TEST(logger_init_success);
  ADD_TEST(default_fmt);

  START_ALL_TETS();
}
