#include "tests.h"

#include <string.h>

#include "../include/qlogger.h"

START_TEST(logger_init_fail)
  void* mem = malloc(72);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, 72, stderr, LOG_LEVEL_DEBUG);

  ASSERT(logger == nullptr);
  free(mem);
END_TEST

START_TEST(logger_init_success)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_DEBUG);

  ASSERT(logger != nullptr);
  free(mem);
END_TEST

START_TEST(f_log_test)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_INFO);
  ASSERT(logger->_fmt_buf[0] != 0);

  LOG_INFO(logger, "long formatted log %d, %zu}", 42, sizeof(int));
  LOG_CRITICAL(logger, "short log %d-", 12);
  LOG_TRACE(logger, "hiden log %d}", 12);

  ASSERT(logger != nullptr);
  free(mem);
END_TEST

START_TEST(log_test)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_WARN);

  LOG_ERROR(logger, "without formatting");
  LOG_INFO(logger, "hiden");
  
  ASSERT(logger != nullptr);
  free(mem);
END_TEST
