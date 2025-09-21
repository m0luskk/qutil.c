#include "tests.h"

#include <stdio.h>
#include <string.h>
#include <threads.h>

#include "../include/qlogger.h"

START_TEST(logger_init_fail)
  void* mem = malloc(72);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, 72, stderr, LOG_LEVEL_DEBUG);

  ASSERT(logger == nullptr);
  free(mem);
END_TEST

START_TEST(logger_init_success)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_DEBUG);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

START_TEST(f_log_test)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_INFO);
  ASSERT(logger->_fmt_buf[0] != 0);

  LOG_INFO(logger, "long formatted log %d, %zu}", 42, sizeof(int));
  LOG_CRITICAL(logger, "short log %d-", 12);
  LOG_TRACE(logger, "hiden log %d}", 12);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

START_TEST(log_test)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_WARN);

  LOG_ERROR(logger, "without formatting");
  LOG_INFO(logger, "hiden");
  
  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

START_TEST(sink_add_fail)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_WARN);
  
  int n =logger_add_sink(logger, sink_get(stdout, LOG_LEVEL_INFO));
  ASSERT(n == -1);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

START_TEST(multy_sink_log)
  void* mem = malloc(logger_min_req_memory + 50);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory + 50, stderr, LOG_LEVEL_WARN);
  
  int n = logger_add_sink(logger, sink_get(stdout, LOG_LEVEL_INFO));
  ASSERT(n == 0);

  ASSERT(logger->_sinks_count == 2);

  LOG_INFO(logger, "one n = %d", n);
  LOG_WARN(logger, "two n = %d", n);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
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
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_INFO);
  
  logger_set_fmt(logger, custom_fmt);

  LOG_INFO(logger, "faskdm %d", 2);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

int thread_func1(void* lg) {
  struct logger* logger = lg;
  for(size_t i = 0; i < 10; ++i) {
    // thrd_sleep(&(struct timespec){.tv_sec= 1}, NULL);
    LOG_DEBUG(logger, "123456789");
  }

  return 0;
}
int thread_func2(void* lg) {
  struct logger* logger = lg;
  for(size_t i = 0; i < 10; ++i) {
    // thrd_sleep(&(struct timespec){.tv_sec= 1}, NULL);
    LOG_INFO(logger, "thread");
  }

  return 0;
}
int thread_func3(void* lg) {
  struct logger* logger = lg;
  for(size_t i = 0; i < 10; ++i) {
    // thrd_sleep(&(struct timespec){.tv_sec= 1}, NULL);
    LOG_WARN(logger, "BLABLA");
  }

  return 0;
}
int thread_func4(void* lg) {
  struct logger* logger = lg;
  for(size_t i = 0; i < 10; ++i) {
    // thrd_sleep(&(struct timespec){.tv_sec= 1}, NULL);
    LOG_WARN(logger, "!message!");
  }

  return 0;
}

START_TEST(logger_mt)
  void* mem = malloc(logger_min_req_memory);
  ASSERT(mem != nullptr);
  auto logger = logger_create(mem, logger_min_req_memory, stderr, LOG_LEVEL_DEBUG);
  
  LOG_DEBUG(logger, "FORMAT, ...");

  //thrd_t threads[4];
  // for (size_t i = 0; i < sizeof(threads) / sizeof(thrd_t); ++i) {
  // thrd_sleep(&(struct timespec){.tv_sec= 1}, NULL);
  // thrd_create(&threads[0], thread_func1, logger);
  // thrd_create(&threads[1], thread_func2, logger);
  // thrd_create(&threads[2], thread_func3, logger);
  // thrd_create(&threads[3], thread_func4, logger);
  

  // for(size_t i = 0; i < 10; ++i) {
  //   LOG_DEBUG(logger, "thread");
  // }

  // for (size_t i = 0; i < sizeof(threads)  / sizeof(thrd_t); ++i) {
  //   thrd_join(threads[i], nullptr);
  // }

  LOG_INFO(logger, "faskdm %d", 2);

  ASSERT(logger != nullptr);
  logger_destroy(logger);
  free(mem);
END_TEST

