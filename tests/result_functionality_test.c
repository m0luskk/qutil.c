#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define R_PEDANTIC_SAFE

#include "tests.h"
#include "../include/qresult.h"
#include "../include/qoption.h"

typedef struct {
  size_t s;
} user_type;

void inspect([[maybe_unused]] double val) {
  printf("inspect: ok value = %f\n", val);
}

struct result_double_serror and_then(double val) {
  return result_double_serror_ok(val * val);
}

struct result_double_serror or_else([[maybe_unused]] serror err) {
  return result_double_serror_ok(4.2);
}

int map(double v) {
  return (int)v;
}

struct option_int opt_or_else() {
  return option_int_value(1);
}

typedef enum {
  DIVISION_BY_ZERO,
  INTEGER_OVERFLOW,
  // ...
} arithm_e;

DECLARE_OPTION(arithm_e)
DECLARE_RESULT(double, arithm_e)

struct result_double_arithm_e divide(double val, double devider) {
  if (devider == 0) {
    return result_double_arithm_e_err(DIVISION_BY_ZERO);
  }
  return result_double_arithm_e_ok(val / devider);
}

struct result_p_void_serror another_fail_function() {
  return result_p_void_serror_err("fail");
}

struct option_int none_int_opt() {
  return option_int_none();
}

struct option_double opt_tries() {
  OPTION_PROPAGATE(double);
  
  auto i = OPT_TRY(none_int_opt());
  
  return option_double_value(i);
}

struct result_double_arithm_e some_arithmetic() {
  ERROR_PROPAGATE(double, arithm_e);

  auto div = RES_TRY(divide(4, 0));

  //auto mul = RES_TRY(multiply(4, 6));

  // ...

  return result_double_arithm_e_ok(div);
}

START_TEST(result_match)
  auto r = some_arithmetic();

  switch (result_match(&r)) {
    case RES_OK:
      ASSERT(r.is_ok);
      printf("ok: %f\n", option_double_unwrap(result_double_arithm_e_get_value(r)));
      break;
    case RES_ERR:
      ASSERT(!r.is_ok);
      printf("error: %d\n", r._value.err); // or some arithm_e to string function
      break;
  }
END_TEST

START_TEST(result_inspect)
  result_double_arithm_e_inspect(some_arithmetic(), inspect);
END_TEST

START_TEST(result_unwrap)
  auto r = result_double_serror_ok(5.2);

  ASSERT(result_double_serror_unwrap(r) > 5.0);
END_TEST

// START_TEST(result_and_then)
//   auto r = result_double_serror_ok(5.2);

//   auto t = result_double_serror_and_then(&r, and_then);

//   ASSERT(result_double_serror_unwrap(&t) > 20.0);
// END_TEST

START_TEST(result_or_else)
  auto r = result_double_serror_err("hui");

  auto t = result_double_serror_or_else(r, or_else);

  ASSERT(result_double_serror_unwrap(t) < 5.2);
END_TEST




START_TEST(option_match)
  auto opt = option_int_value(52);
  ASSERT(option_has_value(&opt));
  ASSERT(opt.has_value);
  ASSERT(opt._value == 52);
  
  switch (option_match(&opt)) {
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
  auto opt = option_int_value(5);
  ASSERT(opt.has_value);

  int i = option_int_unwrap(opt);
  ASSERT(i == 5);
END_TEST

START_TEST(option_unwrap_or)
  auto opt = option_int_none();
  ASSERT(!opt.has_value);

  int i = option_int_unwrap_or(opt, 5);
  ASSERT(i == 5);
END_TEST

START_TEST(option_take)
  auto opt = option_int_value(5);
  ASSERT(opt.has_value);

  auto taken = option_int_take(&opt);
  ASSERT(option_int_unwrap(taken) == 5);
  ASSERT(!opt.has_value)
END_TEST

START_TEST(option_or_else)
  auto value = option_int_value(5);
  ASSERT(value.has_value);
  auto none = option_int_none();
  ASSERT(!none.has_value);

  auto v = option_int_or_else(&value, opt_or_else);
  ASSERT(v._value == 5)
  auto n = option_int_or_else(&none, opt_or_else);
  ASSERT(n._value == 1)
END_TEST

