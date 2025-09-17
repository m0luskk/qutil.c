#include "tests.h"

#include "../include/qlogger.h"
#include <string.h>

START_TEST(logger_init)
  void* mem = malloc(72);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, 72, stderr, LOG_LEVEL_DEBUG);

  printf("%zu\n", logger->_capacity);

  free(mem);
END_TEST

START_TEST(default_fmt)
  time_t now = time(NULL);
  struct tm *local_tm = localtime(&now);
  char buf[128];

  for(size_t i = 0; i < LOG_LEVEL_OFF; ++i) {
    default_fmt(sizeof(buf), buf, local_tm, i);
    strcat(buf, "hui");
    printf("%s\n", buf);
  }

END_TEST
