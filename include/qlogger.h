#ifndef _Q_LOGGER_H
#define _Q_LOGGER_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#define _Q_ANSI_BLACK     "\033[30m"
#define _Q_ANSI_RED       "\033[31m"
#define _Q_ANSI_GREEN     "\033[32m"
#define _Q_ANSI_YELLOW    "\033[33m"
#define _Q_ANSI_BLUE      "\033[34m"
#define _Q_ANSI_MAGENTA   "\033[35m"
#define _Q_ANSI_CYAN      "\033[36m"
#define _Q_ANSI_WHITE     "\033[37m"

#define _Q_ANSI_BG_BLACK   "\033[40m"
#define _Q_ANSI_BG_RED     "\033[41m"
#define _Q_ANSI_BG_GREEN   "\033[42m"
#define _Q_ANSI_BG_YELLOW  "\033[43m"
#define _Q_ANSI_BG_BLUE    "\033[44m"
#define _Q_ANSI_BG_MAGENTA "\033[45m"
#define _Q_ANSI_BG_CYAN    "\033[46m"
#define _Q_ANSI_BG_WHITE   "\033[47m"

#define _Q_ANSI_RESET     "\033[0m"

constexpr size_t _q_color_code_len = 6;

typedef enum q_log_level : uint8_t {
  Q_LOG_LEVEL_OFF = 0,
  Q_LOG_LEVEL_CRITICAL = 1,
  Q_LOG_LEVEL_ERROR = 1 << 1,
  Q_LOG_LEVEL_WARN = 1 << 2,
  Q_LOG_LEVEL_INFO = 1 << 3,
  Q_LOG_LEVEL_DEBUG = 1 << 4,
  Q_LOG_LEVEL_TRACE = 1 << 5,
  _q_log_levels_count,
} q_log_level_t;
static constexpr size_t _q_max_len_of_level_str = 10;

#define _Q_LEVEL_CLZ(LEVEL) UINT8_MAX >> (__builtin_clz(LEVEL) - 24)

constexpr char _q_log_colored_levels_strings[_q_log_levels_count][_q_max_len_of_level_str + _q_color_code_len * 3] = {
  [Q_LOG_LEVEL_OFF] = "OFF",
  [Q_LOG_LEVEL_TRACE] =    _Q_ANSI_WHITE "TRACE" _Q_ANSI_RESET,
  [Q_LOG_LEVEL_DEBUG] =    _Q_ANSI_BLUE "DEBUG"  _Q_ANSI_RESET,
  [Q_LOG_LEVEL_INFO] =     _Q_ANSI_GREEN "INFO"  _Q_ANSI_RESET,
  [Q_LOG_LEVEL_WARN] =     _Q_ANSI_YELLOW "WARN" _Q_ANSI_RESET,
  [Q_LOG_LEVEL_ERROR] =    _Q_ANSI_RED "ERROR"   _Q_ANSI_RESET,
  [Q_LOG_LEVEL_CRITICAL] = _Q_ANSI_WHITE         _Q_ANSI_BG_RED "CRITICAL" _Q_ANSI_RESET,
};

typedef struct q_sink {
  FILE* _stream;
  enum q_log_level _level;
} q_sink_t;

typedef size_t(*q_fmt_f)(size_t str_buf_size, char str_buf[static restrict str_buf_size]);

static constexpr size_t _q_default_fmt_req_size = 16;
static inline size_t _q_default_fmt(size_t count, char str_buf[static restrict count]) {
  if (str_buf == nullptr) return 0;
  if (count < _q_default_fmt_req_size) return 0;

  str_buf[0] = 0;
  strcat(str_buf, "[%s] [%s] %s");

  return strlen(str_buf);
}

constexpr size_t _q_message_buffer_size = 150;
constexpr size_t _q_max_fmt_size = _q_default_fmt_req_size * 2;
typedef struct q_logger {
  char _message_buf[_q_message_buffer_size];
  char _fmt_buf[_q_max_fmt_size];
  void* const _memory;
  q_fmt_f _fmt;
  const size_t _capacity;
  size_t _sinks_count;
  mtx_t _mtx;
  struct q_sink _sinks[];
} q_logger_t;
constexpr size_t q_logger_min_req_memory = sizeof(struct q_logger) + sizeof(struct q_sink);

static inline struct q_sink q_sink_get(FILE* stream, enum q_log_level level) {
  return (struct q_sink) {
    ._level = _Q_LEVEL_CLZ(level),
    ._stream = stream,
  };
}

static inline int q_logger_add_sink(struct q_logger* logger, struct q_sink sink) {
  if (logger == nullptr) return -1;

  mtx_lock(&logger->_mtx);

  size_t req_mem = offsetof(struct q_logger, _sinks) + (logger->_sinks_count + 1) * sizeof(struct q_sink);
  if (logger->_capacity < req_mem) return -1;

  memset(&logger->_sinks[logger->_sinks_count], 0, sizeof(struct q_sink));
  logger->_sinks[logger->_sinks_count] = sink;
  ++logger->_sinks_count;

  mtx_unlock(&logger->_mtx);

  return 0;
}

static inline void q_logger_set_fmt(struct q_logger* restrict logger, q_fmt_f f) {
  if (logger == nullptr) return;
  mtx_lock(&logger->_mtx);
  logger->_fmt = f;
  if(logger->_fmt(sizeof(logger->_fmt_buf), logger->_fmt_buf) > _q_max_fmt_size) {
    mtx_unlock(&logger->_mtx);
    abort(); // format string size exceeds buffer size (_max_fmt_size)
  }
  mtx_unlock(&logger->_mtx);
}

/**
* @brief Creates single threaded logger
* @param fd output stream descriptor
* @param mem pointer to memory space for logger
* @param cap `mem` pointer memory capacity
*/
[[nodiscard]]
static inline struct q_logger* q_logger_create(void* restrict mem, size_t cap, FILE* restrict stream_fd, enum q_log_level level) {
  if (mem == nullptr) return nullptr;

  static constexpr size_t logger_size = offsetof(struct q_logger, _sinks) + sizeof(struct q_sink); 
  if (logger_size > cap) return nullptr;

  memset(mem, 0, logger_size);
  struct q_logger* l = mem;
  memcpy(l, &(struct q_logger){ ._capacity = cap, ._memory = mem, ._sinks_count = 1 }, sizeof(*l));
  memset(l->_message_buf, 0, _q_message_buffer_size);
  l->_sinks[0] = q_sink_get(stream_fd, level);
  mtx_init(&l->_mtx, mtx_plain);

  q_logger_set_fmt(l, _q_default_fmt);

  return l;
}

static inline void q_logger_destroy(struct q_logger* logger) {
  if (!logger) return;
  mtx_destroy(&logger->_mtx);
}

#define Q_LOG_TRACE(LOGGER, FORMAT, ...)    _Q_LOG(LOGGER, Q_LOG_LEVEL_TRACE, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define Q_LOG_DEBUG(LOGGER, FORMAT, ...)    _Q_LOG(LOGGER, Q_LOG_LEVEL_DEBUG, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define Q_LOG_INFO(LOGGER, FORMAT, ...)     _Q_LOG(LOGGER, Q_LOG_LEVEL_INFO, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define Q_LOG_WARN(LOGGER, FORMAT, ...)     _Q_LOG(LOGGER, Q_LOG_LEVEL_WARN, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define Q_LOG_ERROR(LOGGER, FORMAT, ...)    _Q_LOG(LOGGER, Q_LOG_LEVEL_ERROR, FORMAT __VA_OPT__(,) __VA_ARGS__)
#define Q_LOG_CRITICAL(LOGGER, FORMAT, ...) _Q_LOG(LOGGER, Q_LOG_LEVEL_CRITICAL, FORMAT __VA_OPT__(,) __VA_ARGS__)

constexpr size_t _time_str_buf_size = 20;
#define _Q_LOG(LOGGER, LEVEL, FORMAT, ...) \
do { \
  time_t now = time(NULL); \
  struct tm local_tm; \
  localtime_r(&now, &local_tm); \
  char time_str_buf[_time_str_buf_size] = {}; \
  strftime(time_str_buf, sizeof(time_str_buf), "%Y-%m-%d %H:%M:%S", &local_tm); \
  for(size_t i = 0; i < LOGGER->_sinks_count; ++i) { \
    if ((LOGGER->_sinks[i]._level & LEVEL) == LEVEL) { \
      mtx_lock(&LOGGER->_mtx); \
      snprintf(LOGGER->_message_buf, sizeof(LOGGER->_message_buf), LOGGER->_fmt_buf, time_str_buf, _q_log_colored_levels_strings[LEVEL], "" FORMAT "\n"); \
      _Q_FPRINTF(LOGGER->_sinks[i]._stream, LOGGER->_message_buf, __VA_ARGS__); \
      mtx_unlock(&LOGGER->_mtx); \
    } \
  } \
} while (false)

#define _Q_FPRINTF(STREAM, FORMAT, ...) \
  _Q_FPRINTF_ ## __VA_OPT__(FORMATED) (STREAM, FORMAT __VA_OPT__(,) __VA_ARGS__)

#define _Q_FPRINTF_FORMATED(STREAM, FORMAT, ...) fprintf(STREAM, FORMAT, __VA_ARGS__)
#define _Q_FPRINTF_(STREAM, STRING) fputs(STRING, STREAM)


#endif
