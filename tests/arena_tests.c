#include <check.h>

#include "../include/qmemory.h"
#include <stdlib.h>
#include <threads.h>

struct test {
  void* p1;
  void* p2;
  bool b;
};

START_TEST(arena_init) {
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct q_arena arena = q_arena_init(malloc(a_size), a_size);
  ck_assert(arena.destroyed == false);
  ck_assert(arena.capacity == a_size);
  ck_assert(arena._offset == 0);
  ck_assert(arena.data != nullptr);

  q_arena_destroy(&arena, free);
}
END_TEST

START_TEST(arena_destroy) {
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct q_arena arena = q_arena_init(malloc(a_size), a_size);
  q_arena_destroy(&arena, free);
  ck_assert(arena.destroyed == true);
}
END_TEST

START_TEST(arena) {
  struct q_arena arena = q_arena_init(malloc(sizeof(int) * 2 + 2), sizeof(int) * 2 + 2);
  
  int* i = Q_ARENA_ALLOCATE(&arena, int);
  ck_assert(arena._offset == 4);
  ck_assert(i != nullptr);

  int* i2 = Q_ARENA_ALLOCATE(&arena, int);
  ck_assert(arena._offset == 8);
  ck_assert(i2 != nullptr);

  int* i3 = Q_ARENA_ALLOCATE(&arena, int);
  ck_assert(arena._offset == 8);
  ck_assert(i3 == nullptr);

  q_arena_destroy(&arena, free);
}
END_TEST

START_TEST(arena_with_structs) {
  constexpr size_t a_size = sizeof(struct test) * 3;
  struct q_arena arena = q_arena_init(malloc(a_size), a_size);
  
  int* i = Q_ARENA_ALLOCATE(&arena, int);
  ck_assert(arena._offset == 4);
  ck_assert(i != nullptr);

  struct test* t = Q_ARENA_ALLOCATE(&arena, struct test); // successfully allocation
  ck_assert(arena._offset == 32);
  ck_assert(t != nullptr);

  struct test* t2 = Q_ARENA_ALLOCATE(&arena, struct test); // successfully allocation
  ck_assert(arena._offset == 56);
  ck_assert(t2 != nullptr);

  struct test* t3 = Q_ARENA_ALLOCATE(&arena, struct test); // failed allocation
  ck_assert(arena._offset == 56);
  ck_assert(t3 == nullptr);

  q_arena_destroy(&arena, free);
}
END_TEST

START_TEST(subarena) {
  constexpr size_t a_size = KB(1);
  struct q_arena arena = q_arena_init(malloc(a_size), a_size);
  ck_assert(arena.destroyed != true);

  struct q_arena subarena =  q_arena_alloc_subarena(&arena, a_size / 2);

  ck_assert(subarena.destroyed == false);
  ck_assert(subarena.data == arena.data);
  ck_assert(arena._offset == a_size / 2);

  q_arena_destroy(&arena, free);
  ck_assert(arena.destroyed == true);
}
END_TEST

int subth1(void* data) {
  struct q_arena* g_arena = (struct q_arena*)data;

  struct q_arena scratch = q_arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ck_assert(scratch.destroyed == false);
  ck_assert(scratch.capacity == g_arena->capacity / 4);
  // printf("diff = %zu", (unsigned char*)scratch.data - (unsigned char*)g_arena->data);

  return 0;
}

int subth2(void* data) {
  struct q_arena* g_arena = (struct q_arena*)data;

  struct q_arena scratch = q_arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ck_assert(scratch.destroyed == false);
  ck_assert(scratch.capacity == g_arena->capacity / 4);
  // printf("diff = %zu", (unsigned char*)scratch.data - (unsigned char*)g_arena->data);

  return 0;
}

int subth3(void* data) {
  struct q_arena* g_arena = (struct q_arena*)data;

  struct q_arena scratch = q_arena_alloc_subarena(g_arena, g_arena->capacity / 4);

  ck_assert(scratch.destroyed == false);
  ck_assert(scratch.capacity == g_arena->capacity / 4);

  return 0;
}

START_TEST(subarena_mt) {
  constexpr size_t a_size = KB(1);
  struct q_arena arena = q_arena_init(malloc(a_size), a_size);
  ck_assert(arena.destroyed != true);

  thrd_t th[3];
  thrd_create(&th[0], subth1, &arena);
  thrd_create(&th[1], subth2, &arena);
  thrd_create(&th[2], subth3, &arena);

  for(size_t i = 0; i < 3; ++i) thrd_join(th[i], nullptr);

  ck_assert(arena._offset == (arena.capacity / 4) * 3);

  q_arena_reset(&arena);

  ck_assert(arena._offset == 0);

  q_arena_destroy(&arena, free);
  ck_assert(arena.destroyed == true);
}
END_TEST

Suite* arena_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("arena");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, arena_init);
  tcase_add_test(tc_core, arena_destroy);
  tcase_add_test(tc_core, arena_with_structs);
  tcase_add_test(tc_core, subarena);
  tcase_add_test(tc_core, subarena_mt);
  tcase_add_test(tc_core, arena);
  
  suite_add_tcase(s, tc_core);

  return s;
}
