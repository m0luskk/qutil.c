#include "../include/option.h"

bool option_has_value(void* option) {
  return *((bool*)option);
}

enum option_enum option_match(void* option) {
  if (option_has_value(option)) {
    return OPT_VALUE;
  } else {
    return OPT_NONE;
  }
}

IMPL_OPTION(int)

