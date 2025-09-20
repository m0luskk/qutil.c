#pragma once

#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <threads.h>
#include <stdio.h>

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

struct arena {
  void* const data;
  const size_t capacity;
  size_t _offset;
  _Atomic(bool) destroyed;
  atomic_flag _lock;
};

typedef void(*f_arena_data_free)(void* data_pointer);

[[nodiscard]]
static inline struct arena arena_init(void* data, size_t size) {
  if (data == nullptr || size == 0) {
    return (struct arena) { .data = nullptr, .capacity = 0, .destroyed = true, ._lock = ATOMIC_FLAG_INIT };
  }
  return (struct arena) {
    .data = data,
    .capacity = size,
    .destroyed = false,
    ._lock = ATOMIC_FLAG_INIT,
  };
}

#define NULL_ARENA arena_init(nullptr, 0)

static inline void arena_destroy(struct arena* arena, f_arena_data_free free_func) {
  if (arena) {
    while(atomic_flag_test_and_set_explicit(&arena->_lock, memory_order_acquire));

    printf("destroy");

    if(free_func && !atomic_load_explicit(&arena->destroyed, memory_order_relaxed)) {
      free_func(arena->data);
      atomic_store_explicit(&arena->destroyed, true, memory_order_relaxed);
    }

    atomic_flag_clear_explicit(&arena->_lock, memory_order_release);
  }
}

#define ARENA_ALLOCATE(ARENA, TYPE) arena_allocate(ARENA, sizeof(TYPE), alignof(TYPE))

[[nodiscard("pointer to allocated data dropped")]]
static inline void* arena_allocate(struct arena* arena, size_t size, size_t align) {
  if (!arena) goto err_ret;
  while(atomic_flag_test_and_set_explicit(&arena->_lock, memory_order_acquire));
  if (atomic_load_explicit(&arena->destroyed, memory_order_relaxed)) goto err;

  if (size > SIZE_MAX - arena->_offset) goto err;

  size_t aligned_offset = ALIGN_UP(arena->_offset, align);

  if (aligned_offset + size > arena->capacity) goto err;

  void* p = (unsigned char*)arena->data + aligned_offset;

  arena->_offset = aligned_offset + size;

  atomic_flag_clear_explicit(&arena->_lock, memory_order_release);
  return p;
 err:
  atomic_flag_clear_explicit(&arena->_lock, memory_order_release);
 err_ret:
  return nullptr;
}

static inline struct arena arena_get_subarena(struct arena* arena, size_t size) {
  void* data = arena_allocate(arena, size, 1);
  if (!data) return NULL_ARENA;

  return (struct arena) {
    .data = data,
    .capacity = size,
    .destroyed = false,
    ._lock = ATOMIC_FLAG_INIT,
  };
}

// arena_reset
