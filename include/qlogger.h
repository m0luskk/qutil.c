#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum log_level {
  LOG_LEVEL_TRACE,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_CRITICAL,
  LOG_LEVEL_OFF,
};

struct sink {
  FILE* _stream;
  enum log_level _level;
};

struct formatter {};

struct logger {
  void* _memory;
  size_t _capacity;
  struct formatter _formatter;
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

  return l;
}

static inline void logger_basic_st_delete() {}

