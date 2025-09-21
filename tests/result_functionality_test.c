#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define Q_PEDANTIC_SAFE

#include "tests.h"
#include "../include/qresult.h"
#include "../include/qoption.h"

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
  Q_OPTION_PROPAGATE(double);
  
  auto i = Q_OPT_TRY(none_int_opt());
  
  return q_option_double_value(i);
}

struct q_result_double_arithm_e some_arithmetic() {
  Q_ERROR_PROPAGATE(double, arithm_e);

  auto div = Q_RES_TRY(divide(4, 0));

  //auto mul = Q_RES_TRY(multiply(4, 6));

  // ...

  return q_result_double_arithm_e_ok(div);
}

START_TEST(result_match)
  auto r = some_arithmetic();

  switch (q_result_match(&r)) {
    case RES_OK:
      ASSERT(r.is_ok);
      printf("ok: %f\n", Q_OPT_UNWRAP(q_result_double_arithm_e_get_value(r)));
      break;
    case RES_ERR:
      ASSERT(!r.is_ok);
      printf("error: %d\n", Q_OPT_UNWRAP(q_result_double_arithm_e_get_err(r))); 
      break;
  }
END_TEST

START_TEST(result_inspect)
  q_result_double_arithm_e_inspect(some_arithmetic(), inspect);
END_TEST

START_TEST(result_unwrap)
  auto r = q_result_double_serror_ok(5.2);

  ASSERT(q_result_double_serror_unwrap(r) > 5.0);
  ASSERT(Q_RES_UNWRAP(r) > 5.0);
END_TEST

START_TEST(result_or_else)
  auto r = q_result_double_serror_err("hui");

  auto t = q_result_double_serror_or_else(r, or_else);

  ASSERT(q_result_double_serror_unwrap(t) < 5.2);
END_TEST

START_TEST(option_match)
  auto opt = q_option_int_value(52);
  ASSERT(q_option_has_value(&opt));
  ASSERT(opt.has_value);
  ASSERT(opt._value == 52);
  
  switch (q_option_match(&opt)) {
    case OPT_VALUE:
      ASSERT(opt.has_value);
      break;
    case OPT_NONE:
      ASSERT(!opt.has_value);
      break;
  }
END_TEST

START_TEST(option_try)
  auto opt = opt_tries();
  ASSERT(!opt.has_value);
END_TEST

START_TEST(option_unwrap)
  auto opt = q_option_int_value(5);
  ASSERT(opt.has_value);

  int i = Q_OPT_UNWRAP(opt);
  ASSERT(i == 5);
END_TEST

START_TEST(option_unwrap_or)
  auto opt = q_option_int_none();
  ASSERT(!opt.has_value);

  int i = q_option_int_unwrap_or(opt, 5);
  ASSERT(i == 5);
END_TEST

START_TEST(option_take)
  auto opt = q_option_int_value(5);
  ASSERT(opt.has_value);

  auto taken = q_option_int_take(&opt);
  ASSERT(q_option_int_unwrap(taken) == 5);
  ASSERT(!opt.has_value)
END_TEST

START_TEST(option_or_else)
  auto value = q_option_int_value(5);
  ASSERT(value.has_value);
  auto none = q_option_int_none();
  ASSERT(!none.has_value);

  auto v = q_option_int_or_else(value, opt_or_else);
  ASSERT(v._value == 5)
  auto n = q_option_int_or_else(none, opt_or_else);
  ASSERT(n._value == 1)
END_TEST

