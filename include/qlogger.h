#pragma once

#include <stddef.h>
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


enum log_level {
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_CRITICAL,
  LOG_LEVEL_OFF,
};
static constexpr size_t max_len_of_level_str = 10;

#define _LOG_LEVELS_STRINGS   \
(constexpr char[LOG_LEVEL_OFF + 1][max_len_of_level_str]) { \
  [LOG_LEVEL_TRACE] = "TRACE", \
  [LOG_LEVEL_DEBUG] = "DEBUG", \
  [LOG_LEVEL_INFO] = "INFO", \
  [LOG_LEVEL_WARN] = "WARN", \
  [LOG_LEVEL_ERROR] = "ERROR", \
  [LOG_LEVEL_CRITICAL] = "CRITICAL", \
  [LOG_LEVEL_OFF] = "OFF" /* Just for safety */ \
}

#define _LOG_LEVELS_COLORS   \
(constexpr char[LOG_LEVEL_OFF + 1][color_code_len * 2]) { \
  [LOG_LEVEL_TRACE] = ANSI_WHITE, \
  [LOG_LEVEL_DEBUG] = ANSI_BLUE, \
  [LOG_LEVEL_INFO] = ANSI_GREEN, \
  [LOG_LEVEL_WARN] = ANSI_YELLOW, \
  [LOG_LEVEL_ERROR] = ANSI_RED, \
  [LOG_LEVEL_CRITICAL] = ANSI_BLACK ANSI_BG_RED, \
  [LOG_LEVEL_OFF] = "OFF"  /* Just for safety */ \
}

struct sink {
  FILE* _stream;
  enum log_level _level;
};

typedef char*(*fmt_f)(struct tm*, enum log_level);

// todo [[unsequenced]]? (must check strftime() and snprintf() functions)
static inline char* default_fmt(size_t count, char str_buf[static restrict count], struct tm* restrict tm, enum log_level level) {
  static constexpr size_t time_str_size = 20;
  static constexpr size_t level_str_size = 8;
  static constexpr size_t fmt_size = 6 + color_code_len * 3; // for brackets, tabs and colors
  if (str_buf == nullptr || tm == nullptr) return nullptr;
  if (count < time_str_size + level_str_size + fmt_size + 1) return nullptr;
  // Additional character is a whitespace for further strcat's
  
  char time_str_buf[time_str_size] = {};
  strftime(time_str_buf, time_str_size, "%Y-%m-%d %H:%M:%S", tm);

  int n = {};
  if ((n = snprintf(str_buf, time_str_size + level_str_size + fmt_size + 1, "[%s] [%s%s%s] ", time_str_buf, _LOG_LEVELS_COLORS[level], _LOG_LEVELS_STRINGS[level], ANSI_RESET)) < 0) {
    return nullptr;
  }

  return str_buf;
}

struct logger {
  void* _memory;
  size_t _capacity;
  fmt_f _fmt;
  size_t _sinks_count;
  struct sink _sinks[];
};

static inline struct sink sink_get(FILE* stream, enum log_level level) {
  return (struct sink) {
    ._level = level,
    ._stream = stream,
  };
}

/**
* @brief Creates single threaded logger
* @param fd output stream descriptor
* @param mem pointer to memory space for logger
* @param cap `mem` pointer memory capacity
*/
[[nodiscard]]
static inline struct logger* logger_basic_st_create(void* mem, size_t cap, FILE* stream_fd, enum log_level level) {
  if (mem == nullptr) return nullptr;

  // TODO: global constant of minimum required memory capacity
  static constexpr size_t logger_size = offsetof(struct logger, _sinks) + sizeof(struct sink); 
  if (logger_size > cap) return nullptr;

  memset(mem, 0, logger_size);
  struct logger* l = mem;
  l->_capacity = cap;
  l->_memory = mem;
  l->_sinks_count = 1;
  l->_sinks[0] = sink_get(stream_fd, level);
  l->_fmt = nullptr; // if _fmt is nullptr then using default_fmt function for formatting

  return l;
}

static inline void logger_basic_st_delete() {}


