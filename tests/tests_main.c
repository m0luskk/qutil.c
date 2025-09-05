#include <stdio.h>

#define TESTS_MAIN
#include "tests.h"

DECLARE_TEST(result);
// DECLARE_TEST(option);

int main() {
  printf("tests\n");

  ADD_TEST(result);
  // ADD_TEST(option);

  START_ALL_TETS();
}