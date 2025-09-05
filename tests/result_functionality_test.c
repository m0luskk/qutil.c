#include <assert.h>
#include <stdio.h>
#include <stdlib.h>




typedef struct {
  size_t s;
} user_type;

void ins(double val) {
  printf("inspect: ok value = %f\n", val);
}

//DECLARE_RESULT(int, user_type)
#include "tests.h"
#include "../include/result.h"

START_TEST(result)
//struct result_int_user_type res = result_int_user_type_err( (user_type){.s = 42} );
  auto res = result_int_double_err( 5.2 );

  result_int_double_inspect_err(&res, ins);

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

// START_TEST(option)
//   auto opt = option_int_value(52);
//   ASSERT(option_has_value(&opt));
//   ASSERT(opt.has_value);
//   ASSERT(opt._value == 52);
  
//   switch (option_match(&opt)) {
//     case OPT_VALUE:
//       ASSERT(opt.has_value);
//       break;
//     case OPT_NONE:
//       ASSERT(!opt.has_value);
//       break;
//   }
// END_TEST

