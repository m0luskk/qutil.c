#include "tests.h"

#include "../include/qlogger.h"

START_TEST(mf_log_test)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_WARN);

  LOG_ERROR(logger, "another file");
  
  ASSERT(logger != nullptr);
  free(mem);
END_TEST
