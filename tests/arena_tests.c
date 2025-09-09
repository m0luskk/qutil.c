#include "tests.h"

#include "../include/qmemory.h"
#include <stdlib.h>

struct test {
  void* p1;
  void* p2;
  bool b;
};

START_TEST(arena_init)
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct arena arena = arena_init(malloc(a_size), a_size);
  ASSERT(arena.destroyed == false);
  ASSERT(arena.size == a_size);
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
