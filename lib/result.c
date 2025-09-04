#include "../include/result.h"

#include "../include/other.h"

#include <stdlib.h>

bool result_is_ok(void* result) {
  return *((bool*)result);
}
bool result_is_err(void* result) {
  return !result_is_ok(result);
}

enum result_enum result_match(void* result) {
  if (result_is_ok(result)) {
    return RES_OK;
  } else {
    return RES_ERR;
  }
}

#define X(RET, NAME, ARGS, DEF) RET NAME(ARGS) DEF

IMPL_RESULT(int, double)

#undef X
