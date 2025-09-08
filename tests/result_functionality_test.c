#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "../include/result.h"
#include "../include/option.h"

typedef struct {
  size_t s;
} user_type;

void inspect(double val) {
  printf("inspect: ok value = %s\n", val);
}

struct result_double_serror and_then(double val) {
  return result_double_serror_ok(val * val);
}

struct result_double_serror or_else(serror err) {
  return result_double_serror_ok(4.2);
}

int map(double v) {
  return (int)v;
}

struct result_double_serror devide(double val, double devider) {
  if (devider == 0) {
    return result_double_serror_err("devider is zero");
  }
  result_double_serror_ok(val / devider);
}

struct result_p_void_serror another_fail_function() {
  return result_p_void_serror_err("fail");
}

struct result_double_serror foo() {
  ERROR_PROPAGATE(double, serror);

  auto res = TRY(devide(4, 0));

  auto ptr = TRY(another_fail_function());

  return result_double_serror_ok(res);
}

START_TEST(result_match)
  auto r = foo();

  switch (result_match(&r)) {
    case RES_OK:
      ASSERT(r.is_ok);
      printf("ok: %f\n", r._value.ok);
      break;
    case RES_ERR:
      ASSERT(!r.is_ok);
      printf("error: %s\n", r._value.err);
      break;
  }
END_TEST

START_TEST(result_inspect)
  auto r = foo();

  result_double_serror_inspect(&r, inspect);
END_TEST

START_TEST(result_unwrap)
  auto r = result_double_serror_ok(5.2);

  ASSERT(result_double_serror_unwrap(&r) > 5.0);
END_TEST

START_TEST(result_and_then)
  auto r = result_double_serror_ok(5.2);

  auto t = result_double_serror_and_then(&r, and_then);

  ASSERT(result_double_serror_unwrap(&t) > 20.0);
END_TEST

START_TEST(result_or_else)
  auto r = result_double_serror_err("hui");

  auto t = result_double_serror_or_else(&r, or_else);

  ASSERT(result_double_serror_unwrap(&t) < 5.2);
END_TEST




START_TEST(option)
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

