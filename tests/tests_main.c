#include <stdio.h>

#define TESTS_MAIN
#include "tests.h"

DECLARE_TEST(option);
DECLARE_TEST(result_match);
DECLARE_TEST(result_inspect);
DECLARE_TEST(result_unwrap);
DECLARE_TEST(result_and_then);
DECLARE_TEST(result_or_else);

int main() {
  printf("tests\n");

  ADD_TEST(option);
  ADD_TEST(result_match);
  ADD_TEST(result_inspect);
  ADD_TEST(result_unwrap);
  ADD_TEST(result_and_then);
  ADD_TEST(result_or_else);

  START_ALL_TETS();
}