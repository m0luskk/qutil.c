#pragma once

/**
 * @file other.h
 * @brief Some helpful types. 
 * @ingroup PublicAPI
 */


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if __has_attribute(unsequenced)
  #define _UNSQ_ATTR() [[__unsequenced__]]
#else
  #define _UNSQ_ATTR()
#endif

#if __has_attribute(reproducible)
  #define _REPR_ATTR() [[__reproducible__]]
#else
  #define _REPR_ATTR()
#endif

/**
 * @brief Contain static string pointer
 * for use in `option` and `result` types
 */
typedef const char* static_string; 

/**
 * @brief Contain constant `void*`
 * for use in `option` and `result` types
 */
typedef void* p_void;

[[noreturn]]
static inline void abort_with_error(const char err[static 1]) {
  fprintf(stderr, "Abort with error message: %s\n", err);
  abort();
}

[[noreturn]]
static inline void abort_with_error_at(const char msg[static 1], const char file[static 1], size_t line) { // todo
  fprintf(stderr, msg, file, line);
  abort();
}

/**
 * @brief Macros for generate structure for convenient array operating 
 */
#define ARRAY_CONTAINER(TYPE) typedef struct TYPE##_array { const size_t len; TYPE*const data; } TYPE##_array;

ARRAY_CONTAINER(int)
ARRAY_CONTAINER(double)
ARRAY_CONTAINER(char)
