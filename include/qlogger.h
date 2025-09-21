#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ANSI_BLACK     "\033[30m"
#define ANSI_RED       "\033[31m"
#define ANSI_GREEN     "\033[32m"
#define ANSI_YELLOW    "\033[33m"
#define ANSI_BLUE      "\033[34m"
#define ANSI_MAGENTA   "\033[35m"
#define ANSI_CYAN      "\033[36m"
#define ANSI_WHITE     "\033[37m"

#define ANSI_BG_BLACK   "\033[40m"
#define ANSI_BG_RED     "\033[41m"
#define ANSI_BG_GREEN   "\033[42m"
#define ANSI_BG_YELLOW  "\033[43m"
#define ANSI_BG_BLUE    "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN    "\033[46m"
#define ANSI_BG_WHITE   "\033[47m"

#define ANSI_RESET     "\033[0m"

constexpr size_t color_code_len = 6; // TODO: runtime compute

enum log_level : uint8_t {
  LOG_LEVEL_OFF = 0,
  LOG_LEVEL_CRITICAL = 1,
  LOG_LEVEL_ERROR = 1 << 1,
  LOG_LEVEL_WARN = 1 << 2,
  LOG_LEVEL_INFO = 1 << 3,
  LOG_LEVEL_DEBUG = 1 << 4,
  LOG_LEVEL_TRACE = 1 << 5,
  _log_levels_count,
};
static constexpr size_t max_len_of_level_str = 10;

#define _LEVEL_CLZ(LEVEL) UINT8_MAX >> (__builtin_clz(LEVEL) - 24)

constexpr char _log_colored_levels_strings[_log_levels_count][max_len_of_level_str + color_code_len * 3] = {
  [LOG_LEVEL_OFF] = "OFF",
  [LOG_LEVEL_TRACE] = ANSI_WHITE "TRACE" ANSI_RESET,
  [LOG_LEVEL_DEBUG] = ANSI_BLUE "DEBUG" ANSI_RESET,
  [LOG_LEVEL_INFO] = ANSI_GREEN "INFO" ANSI_RESET,
  [LOG_LEVEL_WARN] = ANSI_YELLOW "WARN" ANSI_RESET,
  [LOG_LEVEL_ERROR] = ANSI_RED "ERROR" ANSI_RESET,
  [LOG_LEVEL_CRITICAL] = ANSI_WHITE ANSI_BG_RED "CRITICAL" ANSI_RESET,
};

struct sink {
  FILE* _stream;
  enum log_level _level;
};

typedef size_t(*fmt_f)(size_t str_buf_size, char str_buf[static restrict str_buf_size]);

static constexpr size_t _default_fmt_req_size = 16;
static inline size_t _default_fmt(size_t count, char str_buf[static restrict count]) {
  if (str_buf == nullptr) return 0;
  if (count < _default_fmt_req_size) return 0;

  str_buf[0] = 0;
  strcat(str_buf, "[%s] [%s] %s");

  return strlen(str_buf);
}

constexpr size_t _message_buffer_size = 150;
constexpr size_t _max_fmt_size = _default_fmt_req_size * 2;
struct logger {
  char _message_buf[_message_buffer_size];
  char _fmt_buf[_max_fmt_size];
  void* const _memory;
  fmt_f _fmt;
  const size_t _capacity;
  size_t _sinks_count;
  struct sink _sinks[];
};
constexpr size_t logger_min_req_memory = sizeof(struct logger) + sizeof(struct sink);

static inline struct sink sink_get(FILE* stream, enum log_level level) {
  return (struct sink) {
    ._level = _LEVEL_CLZ(level),
    ._stream = stream,
  };
}

static inline int logger_add_sink(struct logger* logger, struct sink sink) {
  if (logger == nullptr) return -1;
  size_t req_mem = offsetof(struct logger, _sinks) + (logger->_sinks_count + 1) * sizeof(struct sink);
  if (logger->_capacity < req_mem) return -1;

  memset(&logger->_sinks[logger->_sinks_count], 0, sizeof(struct sink));
  logger->_sinks[logger->_sinks_count] = sink;
  ++logger->_sinks_count;

  return 0;
}

static inline void logger_set_fmt(struct logger* logger, fmt_f f) {
  if (logger == nullptr) return;
  logger->_fmt = f;
  if(logger->_fmt(sizeof(logger->_fmt_buf), logger->_fmt_buf) > _max_fmt_size) {
    abort(); // Your format string size exceeds buffer size (_max_fmt_size)!
  }
}

/**
* @brief Creates single threaded logger
* @param fd output stream descriptor
* @param mem pointer to memory space for logger
* @param cap `mem` pointer memory capacity
*/
[[nodiscard]]
static inline struct logger* logger_basic_st_create(void* restrict mem, size_t cap, FILE* restrict stream_fd, enum log_level level) {
  if (mem == nullptr) return nullptr;

  static constexpr size_t logger_size = offsetof(struct logger, _sinks) + sizeof(struct sink); 
  if (logger_size > cap) return nullptr;

  memset(mem, 0, logger_size);
  struct logger* l = mem;
  memcpy(l, &(struct logger){ ._capacity = cap, ._memory = mem, ._sinks_count = 1 }, sizeof(*l));
  memset(l->_message_buf, 0, _message_buffer_size);
  l->_sinks[0] = sink_get(stream_fd, level);

  logger_set_fmt(l, _default_fmt);

  return l;
}

#define LOG_TRACE(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_TRACE, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_DEBUG, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_INFO, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_WARN, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_ERROR, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define LOG_CRITICAL(LOGGER, FORMAT, ...) _LOG(LOGGER, LOG_LEVEL_CRITICAL, FORMAT __VA_OPT__(,) __VA_ARGS__)

constexpr size_t _time_str_buf_size = 20;
#define _LOG(LOGGER, LEVEL, FORMAT, ...) \
do { \
  time_t now = time(NULL); \
  struct tm *local_tm = localtime(&now); /* localtime is not thread-safe */ \
  char time_str_buf[_time_str_buf_size] = {}; \
  strftime(time_str_buf, sizeof(time_str_buf), "%Y-%m-%d %H:%M:%S", local_tm); \
  for(size_t i = 0; i < LOGGER->_sinks_count; ++i) { \
    if ((LOGGER->_sinks[i]._level & LEVEL) == LEVEL) { \
      /* snprintf is using shared buffer - it is not thread safe */ \
      snprintf(LOGGER->_message_buf, sizeof(LOGGER->_message_buf), LOGGER->_fmt_buf, time_str_buf, _log_colored_levels_strings[LEVEL], "" FORMAT "\n"); \
      _FPRINTF(LOGGER->_sinks[i]._stream, LOGGER->_message_buf, __VA_ARGS__); \
    } \
  } \
} while (false)

#define _FPRINTF(STREAM, FORMAT, ...) \
  _FPRINTF_ ## __VA_OPT__(FORMATED) (STREAM, FORMAT __VA_OPT__(,) __VA_ARGS__)

#define _FPRINTF_FORMATED(STREAM, FORMAT, ...) fprintf(STREAM, FORMAT, __VA_ARGS__)
#define _FPRINTF_(STREAM, STRING) fputs(STRING, STREAM)


// static inline void logger_basic_st_delete() {}


