#include "tests.h"

#include <stdio.h>
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

START_TEST(sink_add_fail)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_WARN);
  
  int n =logger_add_sink(logger, sink_get(stdout, LOG_LEVEL_INFO));
  ASSERT(n == -1);

  ASSERT(logger != nullptr);
  free(mem);
END_TEST

START_TEST(multy_sink_log)
  void* mem = malloc(logger_min_req_memory + 50);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory + 50, stderr, LOG_LEVEL_WARN);
  
  int n = logger_add_sink(logger, sink_get(stdout, LOG_LEVEL_INFO));
  ASSERT(n == 0);

  ASSERT(logger->_sinks_count == 2);

  LOG_INFO(logger, "one n = %d", n);
  LOG_WARN(logger, "two n = %d", n);

  ASSERT(logger != nullptr);
  free(mem);
END_TEST

size_t custom_fmt(size_t str_buf_size, char str_buf[static restrict str_buf_size]) {
  str_buf[0] = 0;
  strcat(str_buf, ";%s@kajdkjk asdjnas^%s? %s");

  return strlen(str_buf);
}

START_TEST(logger_custom_fmt)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_basic_st_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_INFO);
  
  logger_set_fmt(logger, custom_fmt);

  LOG_INFO(logger, "faskdm %d", 2);

  ASSERT(logger != nullptr);
  free(mem);
END_TEST
