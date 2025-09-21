#ifndef _Q_OTHER_H
#define _Q_OTHER_H

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
  #define _Q_UNSQ_ATTR()
#endif

#if __has_attribute(reproducible)
  #define _REPR_ATTR() [[__reproducible__]]
#else
  #define _Q_REPR_ATTR()
#endif

/**
 * @brief Contain static string pointer
 * for use in `option` and `result` types
 */
typedef const char* q_static_string; 

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
