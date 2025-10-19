#define Q_PEDANTIC_SAFE

#include <stdio.h>
#include <stdlib.h>

#include "../include/qresult.h"
#include "../include/qoption.h"

#include <check.h>

typedef struct {
  size_t s;
} user_type;

void inspect([[maybe_unused]] double val) {
  printf("inspect: ok value = %f\n", val);
}

struct q_result_double_serror and_then(double val) {
  return q_result_double_serror_ok(val * val);
}

struct q_result_double_serror or_else([[maybe_unused]] serror err) {
  return q_result_double_serror_ok(4.2);
}

int map(double v) {
  return (int)v;
}

struct q_option_int opt_or_else() {
  return q_option_int_value(1);
}

typedef enum {
  DIVISION_BY_ZERO,
  INTEGER_OVERFLOW,
  // ...
} arithm_e;

Q_DECLARE_OPTION(arithm_e)
Q_DECLARE_RESULT(double, arithm_e)

struct q_result_double_arithm_e divide(double val, double devider) {
  if (devider == 0) {
    return q_result_double_arithm_e_err(DIVISION_BY_ZERO);
  }
  return q_result_double_arithm_e_ok(val / devider);
}

struct q_result_void_p_serror another_fail_function() {
  return q_result_void_p_serror_err("fail");
}

struct q_option_int none_int_opt() {
  return q_option_int_none();
}

struct q_option_double opt_tries() {
  Q_OPTION_CONTEXT(double);
  
  auto i = Q_OPT_TRY(none_int_opt());
  
  return q_option_double_value(i);
}

struct q_result_double_arithm_e some_arithmetic() {
  Q_RESULT_CONTEXT(double, arithm_e);

  auto div = Q_RES_TRY(divide(4, 0));

  //auto mul = Q_RES_TRY(multiply(4, 6));

  // ...

  return q_result_double_arithm_e_ok(div);
}

START_TEST(result_match) {
  auto r = some_arithmetic();

  switch (q_result_match(&r)) {
    case RES_OK:
      ck_assert(r.is_ok);
      printf("ok: %f\n", Q_OPT_UNWRAP(q_result_double_arithm_e_get_value(r)));
      break;
    case RES_ERR:
      ck_assert(!r.is_ok);
      printf("error: %d\n", Q_OPT_UNWRAP(q_result_double_arithm_e_get_err(r))); 
      break;
  }
}
END_TEST

START_TEST(result_inspect) {
  q_result_double_arithm_e_inspect(some_arithmetic(), inspect);
}
END_TEST

START_TEST(result_unwrap) {
  auto r = q_result_double_serror_ok(5.2);

  double* d = Q_RES_UNWRAP_MUT(&r);
  ck_assert(d != nullptr);
  ck_assert(*d > 5.0);

  ck_assert(q_result_double_serror_unwrap(r) > 5.0);
  ck_assert(Q_RES_UNWRAP(r) > 5.0);
}
END_TEST

START_TEST(result_or_else) {
  auto r = q_result_double_serror_err("hui");

  auto t = q_result_double_serror_or_else(r, or_else);

  ck_assert(q_result_double_serror_unwrap(t) < 5.2);
}
END_TEST

START_TEST(option_match) {
  auto opt = q_option_int_value(52);
  ck_assert(q_option_has_value(&opt));
  ck_assert(opt.has_value);
  ck_assert(opt._value == 52);
  
  switch (q_option_match(&opt)) {
    case OPT_VALUE:
      ck_assert(opt.has_value);
      break;
    case OPT_NONE:
      ck_assert(!opt.has_value);
      break;
  }
}
END_TEST

START_TEST(option_try) {
  auto opt = opt_tries();
  ck_assert(!opt.has_value);
}
END_TEST

START_TEST(option_unwrap) {
  auto opt = q_option_int_value(5);
  ck_assert(opt.has_value);

  int i = Q_OPT_UNWRAP(opt);

  int* ip = Q_OPT_UNWRAP_MUT(&opt);
  ck_assert(ip != nullptr);
  ck_assert(*ip == 5);
  ck_assert(i == 5);
}
END_TEST

START_TEST(option_unwrap_or) {
  auto opt = q_option_int_none();
  ck_assert(!opt.has_value);

  int i = q_option_int_unwrap_or(opt, 5);
  ck_assert(i == 5);
}
END_TEST

START_TEST(option_take) {
  auto opt = q_option_int_value(5);
  ck_assert(opt.has_value);

  auto taken = q_option_int_take(&opt);
  ck_assert(q_option_int_unwrap(taken) == 5);
  ck_assert(!opt.has_value);
}
END_TEST

START_TEST(option_or_else) {
  auto value = q_option_int_value(5);
  ck_assert(value.has_value);
  auto none = q_option_int_none();
  ck_assert(!none.has_value);

  auto v = q_option_int_or_else(value, opt_or_else);
  ck_assert(v._value == 5);
  auto n = q_option_int_or_else(none, opt_or_else);
  ck_assert(n._value == 1);
}
END_TEST

Suite* result_option_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("result_option");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, option_or_else);
  tcase_add_test(tc_core, option_match);
  tcase_add_test(tc_core, option_take);
  tcase_add_test(tc_core, option_try);
  tcase_add_test(tc_core, option_unwrap);
  tcase_add_test(tc_core, option_unwrap_or);
  tcase_add_test(tc_core, result_or_else);
  tcase_add_test(tc_core, result_unwrap);
  tcase_add_test(tc_core, result_match);
  tcase_add_test(tc_core, result_inspect);
  
  suite_add_tcase(s, tc_core);

  return s;
}


