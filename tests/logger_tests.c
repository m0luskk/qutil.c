#include "tests.h"

#include "../include/qlogger.h"

START_TEST(logger_init) // LOL asan and valgrind dont see this leak
  void* mem = malloc(72);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, 72, stderr, LOG_LEVEL_DEBUG);
  printf("%zu\n", logger->_capacity);
END_TEST
