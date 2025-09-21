#include "tests.h"

#include "../include/qmemory.h"
#include <stdlib.h>
#include <threads.h>

struct test {
  void* p1;
  void* p2;
  bool b;
};

START_TEST(arena_init)
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct arena arena = arena_init(malloc(a_size), a_size);
  ASSERT(arena.destroyed == false);
  ASSERT(arena.capacity == a_size);
  ASSERT(arena._offset == 0);
  ASSERT(arena.data != nullptr);

  arena_destroy(&arena, free);
END_TEST

START_TEST(arena_destroy)
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct arena arena = arena_init(malloc(a_size), a_size);
  arena_destroy(&arena, free);
  ASSERT(arena.destroyed == true);
END_TEST

START_TEST(arena)
  struct arena arena = arena_init(malloc(sizeof(int) * 2 + 2), sizeof(int) * 2 + 2);
  
  int* i = ARENA_ALLOCATE(&arena, int);
  ASSERT(arena._offset == 4);
  ASSERT(i != nullptr);

  int* i2 = ARENA_ALLOCATE(&arena, int);
  ASSERT(arena._offset == 8);
  ASSERT(i2 != nullptr);

  int* i3 = ARENA_ALLOCATE(&arena, int);
  ASSERT(arena._offset == 8);
  ASSERT(i3 == nullptr);

  arena_destroy(&arena, free);

END_TEST

START_TEST(arena_with_structs)
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct arena arena = arena_init(malloc(a_size), a_size);
  
  int* i = ARENA_ALLOCATE(&arena, int);
  ASSERT(arena._offset == 4);
  ASSERT(i != nullptr);

  struct test* t = ARENA_ALLOCATE(&arena, struct test); // successfully allocation
  ASSERT(arena._offset == 32);
  ASSERT(t != nullptr);

  struct test* t2 = ARENA_ALLOCATE(&arena, struct test); // successfully allocation
  ASSERT(arena._offset == 56);
  ASSERT(t2 != nullptr);

  struct test* t3 = ARENA_ALLOCATE(&arena, struct test); // failed allocation
  ASSERT(arena._offset == 56);
  ASSERT(t3 == nullptr);

  arena_destroy(&arena, free);

END_TEST

START_TEST(subarena)
  constexpr size_t a_size = KB(1);
  struct arena arena = arena_init(malloc(a_size), a_size);
  ASSERT(arena.destroyed != true);

  struct arena subarena =  arena_alloc_subarena(&arena, a_size / 2);

  ASSERT(subarena.destroyed == false);
  ASSERT(subarena.data == arena.data);
  ASSERT(arena._offset == a_size / 2);

  arena_destroy(&arena, free);
  ASSERT(arena.destroyed == true);
END_TEST

int subth1(void* data) {
  struct arena* g_arena = (struct arena*)data;

  struct arena scratch = arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ASSERT(scratch.destroyed == false);
  ASSERT(scratch.capacity == g_arena->capacity / 4);
  printf("diff = %zu", (unsigned char*)scratch.data - (unsigned char*)g_arena->data);

  return 0;
}

int subth2(void* data) {
  struct arena* g_arena = (struct arena*)data;

  struct arena scratch = arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ASSERT(scratch.destroyed == false);
  ASSERT(scratch.capacity == g_arena->capacity / 4);
  printf("diff = %zu", (unsigned char*)scratch.data - (unsigned char*)g_arena->data);

  return 0;
}

int subth3(void* data) {
  struct arena* g_arena = (struct arena*)data;

  struct arena scratch = arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ASSERT(scratch.destroyed == false);
  ASSERT(scratch.capacity == g_arena->capacity / 4);

  return 0;
}

START_TEST(subarena_mt)
  constexpr size_t a_size = KB(1);
  struct arena arena = arena_init(malloc(a_size), a_size);
  ASSERT(arena.destroyed != true);

  thrd_t th[3];
  thrd_create(&th[0], subth1, &arena);
  thrd_create(&th[1], subth2, &arena);
  thrd_create(&th[2], subth3, &arena);

  for(size_t i = 0; i < 3; ++i) thrd_join(th[i], nullptr);

  ASSERT(arena._offset == (arena.capacity / 4) * 3)

  arena_reset(&arena);

  ASSERT(arena._offset == 0);

  arena_destroy(&arena, free);
  ASSERT(arena.destroyed == true);
END_TEST
