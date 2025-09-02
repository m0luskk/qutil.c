#pragma once

#include <stdio.h>
#include <stddef.h>

static constexpr size_t test_reg_size = 32;
static int(*test_reg[test_reg_size])(void) = {};
static char const* test_names_reg[test_reg_size] = {};
static size_t pos = 0;

static void add_test_to_reg(int(*t_f)(void)) {
  if (pos < test_reg_size) {
    test_reg[pos] = t_f;
    ++pos;
  }
}

static void add_test_name_to_reg(const char name[static 1]) {
  test_names_reg[pos] = name;
}

// TODO: print corresponding tests names
static void start_all_tests_in_reg() {
  for (size_t i = 0; i < test_reg_size; ++i) {
    if (test_reg[i]) {
      printf("Test \'%s\': %s\n", test_names_reg[i], (test_reg[i])() ? "SUCCESS" : "FAILED");
    } 
  }
}