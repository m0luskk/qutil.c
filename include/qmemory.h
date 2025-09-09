#pragma once

#include "stdlib.h"
#include <stdint.h>

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

struct arena {
  void* data;
  size_t size;
  bool destroyed;
  size_t _offset;
};

typedef void(*f_arena_data_free)(void* data_pointer);

[[nodiscard("pointer to allocated data dropped")]]
static inline struct arena arena_init(void* data, size_t size) {
  if (data == nullptr) {
    return (struct arena) { .data = nullptr, .size = 0, .destroyed = true };
  }
  return (struct arena) {
    .data = data,
    .size = size,
    .destroyed = false,
  };
}

static inline void arena_destroy(struct arena* arena, f_arena_data_free free_func) {
  free_func(arena->data);
  *arena = (struct arena) { .destroyed = true };
}

#define ARENA_ALLOCATE(ARENA, TYPE) arena_allocate(ARENA, sizeof(TYPE), alignof(TYPE))

[[nodiscard]]
static inline void* arena_allocate(struct arena* arena, size_t size, size_t align) {
  if (arena->destroyed) return nullptr;
  if (size > SIZE_MAX - arena->_offset) return nullptr;

  size_t aligned_offset = ALIGN_UP(arena->_offset, align);
  if (aligned_offset + size > arena->size) return nullptr;

  void* p = (unsigned char*)arena->data + aligned_offset;

  arena->_offset = aligned_offset + size;

  return p;
}
