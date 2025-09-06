#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "../include/result.h"
#include "../include/option.h"

typedef struct {
  size_t s;
} user_type;

void ins(serror val) {
  printf("inspect: ok value = %s\n", val);
}

START_TEST(result)
  auto res = result_int_serror_ok(52);

  auto val = result_int_serror_get_value(&res);

  printf("%d\n", option_has_value(&val));

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

