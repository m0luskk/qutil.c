#include "../include/other.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

[[noreturn]]
void abort_with_error(const char err[static 1]) {
  fprintf(stderr, "Abort with error message: %s\n", err);
  abort();
}

[[noreturn]]
void abort_with_error_at(const char msg[static 1], const char file[static 1], size_t line) { // todo
  fprintf(stderr, msg, file, line);
  abort();
}
