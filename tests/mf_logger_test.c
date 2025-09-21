#include "tests.h"

#include "../include/qlogger.h"

START_TEST(mf_log_test)
  void* mem = malloc(q_logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = q_logger_create(mem, q_logger_min_req_memory, stderr, Q_LOG_LEVEL_WARN);

  Q_LOG_ERROR(logger, "another file");
  
  ASSERT(logger != nullptr);
  q_logger_destroy(logger);
  free(mem);
END_TEST
