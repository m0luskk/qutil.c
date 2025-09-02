#pragma once

#include <stdio.h>

#ifdef TESTS_MAIN
#include "test_register.h"
#endif

#define SUCCESS_CODE 1
#define FAIL_CODE 0

// TODO make it more comfortable
#define DECLARE_TEST(NAME) extern int test##NAME()
#define ADD_TEST(NAME) do { add_test_name_to_reg(#NAME); add_test_to_reg(test##NAME); } while (0)
#define START_ALL_TETS() start_all_tests_in_reg()

#define START_TEST(NAME) int test##NAME() {
#define END_TEST return SUCCESS_CODE; }

#define ASSERT(EXPR) /* */ \
if (!(EXPR)) { \
  printf("FAILED: %s:%d (%s)\n", __FILE__, __LINE__, #EXPR); \
  return FAIL_CODE; \
}

