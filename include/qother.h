#ifndef _Q_OTHER_H
#define _Q_OTHER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
* @file other.h
 * @brief Some helpful types. 
 * @ingroup PublicAPI
 */

#if !defined(_Q_NODISCARD_ATTR) || !defined(Q_NODISCARD_ATTR)
  #if defined(__GNUC__) && (__STDC_VERSION__ < 202311L) 
    #define _Q_NODISCARD_ATTR __attribute__((warn_unused_result))
  #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L) 
    #define _Q_NODISCARD_ATTR [[nodiscard]]
  #else
    #define _Q_NODISCARD_ATTR
  #endif
#endif


#if __has_attribute(unsequenced)
#define Q_UNSQ_ATTR() [[__unsequenced__]]
#else
  #define Q_UNSQ_ATTR()
#endif

#if __has_attribute(reproducible)
#define Q_REPR_ATTR() [[__reproducible__]]
#else
#define Q_REPR_ATTR()
#endif

#define _Q_ABORT_HERE() \
do { \
  fprintf(stderr, "\naborted at %s:%d\n", __FILE__, __LINE__); \
  abort(); \
} while(0);\

typedef const char* cstring; 
typedef char* string;

/**
 * @brief Contain constant `void*`
 * for use in `option` and `result` types
 */
typedef void* void_p;

/**
 * @brief Macros for generate structure for convenient array operating 
 */
#define Q_ARRAY_CONTAINER(TYPE) typedef struct TYPE##_array { const size_t len; TYPE*const data; } TYPE##_array;

Q_ARRAY_CONTAINER(int)
Q_ARRAY_CONTAINER(double)
Q_ARRAY_CONTAINER(char)


#endif
