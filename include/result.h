#pragma once

/**
 * @defgroup PublicAPI public api
 * @brief
 */

/**
 * @defgroup InternalAPI internals
 * @brief
 * @internal
 */

/**
 * @file result.h
 * @brief Result type. 
 * @ingroup PublicAPI
 * Result is a type that represents either success (`ok`) or failure (`err`).
 */

// /**
//  * @ingroup PublicAPI
//  * @brief Функция для пользователя.
//  */
// void hello_world();


#include <stdio.h>
#include "../include/other.h"

/**
 * @ingroup PublicAPI
 * @brief Declare result structure of types `T` as `ok` value and `ERR` as `err` value.
 */
#define DECLARE_RESULT(T, ERR) \
struct result_##T##_##ERR { \
  bool is_ok; \
  union { \
    T ok; \
    ERR err; \
  } _value; \
}; \
struct result_##T##_##ERR result_##T##_##ERR##_ok(T value); \
struct result_##T##_##ERR result_##T##_##ERR##_err(ERR err_value); \
ERR result_##T##_##ERR##_get_err(struct result_##T##_##ERR* result); \
ERR result_##T##_##ERR##_get_value(struct result_##T##_##ERR* result);

#define IMPL_RESULT(T, ERR) \
struct result_##T##_##ERR result_##T##_##ERR##_ok(T value) { \
  return (struct result_##T##_##ERR){ \
    .is_ok = true, \
    ._value.ok = value \
  }; \
} \
struct result_##T##_##ERR result_##T##_##ERR##_err(ERR err_value) { \
  return (struct result_##T##_##ERR){ \
    .is_ok = false, \
    ._value.err = err_value \
  }; \
} \
/*USE `RESULT_GET_ERROR` MACROS INSTEAD*/ \
ERR result_##T##_##ERR##_get_err(struct result_##T##_##ERR* result) { \
 if (!result_is_err(result)) { \
    abort(); \
 } \
 return result->_value.err; \
} \
ERR result_##T##_##ERR##_get_value(struct result_##T##_##ERR* result) { \
 if (!result_is_ok(result)) { \
    abort(); \
 } \
 return result->_value.err; \
} 

/**
 * @ingroup PublicAPI
 * @brief Macros-wrapper of result_T_ERR_get_err function with context of aborting in fail case
 * @return 
 */
#define RESULT_GET_ERROR(T, ERR, RESULT) \
( result_is_err(RESULT) ? result_##T##_##ERR##_get_err(RESULT) \
  : (abort_with_error_at("An attempt to access an inactive `err` value of the result structure at %s:%zu\n", __FILE__, __LINE__), (ERR){}) );

#define RESULT_GET_VALUE(T, ERR, RESULT) \
( result_is_ok(RESULT) ? result_##T##_##ERR##_get_value(RESULT) \
  : (abort_with_error_at("An attempt to access an inactive `ok` value of the result structure at %s:%zu\n", __FILE__, __LINE__), (ERR){}) );

enum result_enum {
  RES_OK,
  RES_ERR,
};

enum result_enum result_match(void* result);

/**
 * @ingroup PublicAPI
 * @brief Returns `true` if the passed result instance contain `ok` value.
 * 
 * @param result Instance of any result structure
 * @return true if passed result instance contain `ok` value
 * @return false if passed result instance contain `err` value
 */
bool result_is_ok(void* result);

/**
 * @ingroup PublicAPI
 * @brief Returns `true` if the passed result instance contain `err` value.
 * 
 * @param result Instance of any result structure
 * @return true if passed result instance contain `err` value
 * @return false if passed result instance contain `ok` value
 */
bool result_is_err(void* result);

DECLARE_RESULT(int, double)

