#include <assert.h>
#include <stdio.h>

#include "tests.h"

#include "../include/result.h"
#include "../include/option.h"

START_TEST(result)
  auto res = result_int_double_ok(52);

  // double s = result_int_double_get_err(&res);
  double s = RESULT_GET_VALUE(int, double, &res);
  (void)s; // unused

  printf("%f\n", s);

  //ASSERT(res.is_ok);
  //ASSERT(res._value.ok == 52);

  switch (result_match(&res)) {
    case RES_OK:
      ASSERT(res.is_ok);
      break;
    case RES_ERR:
      ASSERT(!res.is_ok);
      break;
  }
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

