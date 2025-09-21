#pragma once

#include <stdlib.h>
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>
#include <stdio.h>

#define _Q_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

#define KB(X) ((X)*1024)
#define MB(X) ((X)*1024*1024)

typedef struct q_arena {
  void* const data;
  const size_t capacity;
  size_t _offset;
  _Atomic(bool) destroyed;
  atomic_flag _lock;
} q_arena_t;

typedef void(*q_arena_data_free_f)(void* data_pointer);

[[nodiscard]]
static inline struct q_arena q_arena_init(void* data, size_t size) {
  if (data == nullptr || size == 0) {
    return (struct q_arena) { .data = nullptr, .capacity = 0, .destroyed = true, ._lock = ATOMIC_FLAG_INIT };
  }
  memset(data, 0, size);
  return (struct q_arena) {
    .data = data,
    .capacity = size,
    .destroyed = false,
    ._lock = ATOMIC_FLAG_INIT,
  };
}

#define Q_NULL_ARENA q_arena_init(nullptr, 0)

static inline void q_arena_destroy(struct q_arena* arena, q_arena_data_free_f free_func) {
  if (arena) {
    while(atomic_flag_test_and_set_explicit(&arena->_lock, memory_order_acquire));

    if(free_func && !atomic_load_explicit(&arena->destroyed, memory_order_relaxed)) {
      free_func(arena->data);
      atomic_store_explicit(&arena->destroyed, true, memory_order_relaxed);
    }

    atomic_flag_clear_explicit(&arena->_lock, memory_order_release);
  }
}

#define Q_ARENA_ALLOCATE(ARENA, TYPE) q_arena_alloc(ARENA, sizeof(TYPE), alignof(TYPE))

[[nodiscard("pointer to allocated data dropped")]]
static inline void* q_arena_alloc(struct q_arena* arena, size_t size, size_t align) {
  if (!arena) goto err_ret;
  while(atomic_flag_test_and_set_explicit(&arena->_lock, memory_order_acquire));
  if (atomic_load_explicit(&arena->destroyed, memory_order_relaxed)) goto err;

  if (size > SIZE_MAX - arena->_offset) goto err;

  size_t aligned_offset = _Q_ALIGN_UP(arena->_offset, align);

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

[[nodiscard]]
static inline struct q_arena q_arena_alloc_subarena(struct q_arena* arena, size_t size) {
  void* data = q_arena_alloc(arena, size, 1);
  if (!data) return Q_NULL_ARENA;

  return (struct q_arena) {
    .data = data,
    .capacity = size,
    .destroyed = false,
    ._lock = ATOMIC_FLAG_INIT,
  };
}

static inline void q_arena_reset(struct q_arena* arena) {
  while(atomic_flag_test_and_set_explicit(&arena->_lock, memory_order_acquire));
  memset(arena->data, 0, arena->capacity);
  arena->_offset = 0;
  atomic_flag_clear_explicit(&arena->_lock, memory_order_release);
}
