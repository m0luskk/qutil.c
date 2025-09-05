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
 * ## User-defined result type
 * \attention `user_ok_type` and `user_err_type` must not contain whitespaces and special characters (`const char *` or `int[]` for example)
 
 * ```c
// my_result.h
#define X(RET, NAME, ARGS, DEF) RET NAME(ARGS);
DECLARE_RESULT(user_ok_type, user_err_type);
#undef X

// my_result.c
#define X(RET, NAME, ARGS, DEF) RET NAME(ARGS) DEF
IMPL_RESULT(user_ok_type, user_err_type);
#undef X
 * ```
 */

// /**
//  * @ingroup PublicAPI
//  * @brief Функция для пользователя.
//  */
// void hello_world();

#include <stdlib.h>

#include "other.h"

#if __has_attribute(unsequenced)
  #define UNSEQUENCED_ATTR() [[unsequenced]]
#else
  #define UNSEQUENCED_ATTR()
#endif

#define RESULT_OK_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = true, ._value.ok = value }; }

#define RESULT_ERR_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = false, ._value.err = err_value }; }

#define RESULT_GET_ERR_BODY(T, ERR) { return (result->is_ok ? (abort(), (ERR){}) : result->_value.err); }

#define RESULT_GET_VALUE_BODY(T, ERR) { return (!result->is_ok ? (abort(), (T){}) : result->_value.ok); }

#define RESULT_INSPECT_BODY(T, ERR) { if (result->is_ok) f(result->_value.ok); }
#define RESULT_INSPECT_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_inspect f

#define RESULT_INSPECT_ERR_BODY(T, ERR) { if (!result->is_ok) f(result->_value.err); }
#define RESULT_INSPECT_ERR_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_inspect_err f

// R_M(ATTR, RET, NAME, ARGS, BODY)
#define RESULT_METHODS(T, ERR) \
  R_M(, struct result_##T##_##ERR, result_##T##_##ERR##_ok, T value, RESULT_OK_BODY(T, ERR)) \
  R_M(, struct result_##T##_##ERR, result_##T##_##ERR##_err, ERR err_value, RESULT_ERR_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), ERR, result_##T##_##ERR##_get_err, struct result_##T##_##ERR* result, RESULT_GET_ERR_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), T, result_##T##_##ERR##_get_value, struct result_##T##_##ERR* result, RESULT_GET_VALUE_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), void, result_##T##_##ERR##_inspect, RESULT_INSPECT_ARGS(T, ERR), RESULT_INSPECT_BODY(T, ERR)) \
  R_M(UNSEQUENCED_ATTR(), void, result_##T##_##ERR##_inspect_err, RESULT_INSPECT_ERR_ARGS(T, ERR), RESULT_INSPECT_ERR_BODY(T, ERR))

#define R_M(ATTR, RET, NAME, ARGS, DEF) ATTR static inline RET NAME(ARGS) DEF

/**
 * @ingroup PublicAPI
 * @brief Declare result structure of types `T` as `ok` variant and `ERR` as `err` variant.
 */
#define DECLARE_RESULT(T, ERR) \
struct result_##T##_##ERR { \
  bool is_ok; \
  union { \
    T ok; \
    ERR err; \
  } _value; \
}; \
typedef void(*f_result_##T##_##ERR##_inspect_err)(ERR); \
typedef void(*f_result_##T##_##ERR##_inspect)(T); \
RESULT_METHODS(T, ERR)

/**
 * @brief Contain static error string
 */
typedef static_string serror;

/**
 * @brief Make `serror` instance
 */
#define MAKE_ERROR_MSG(STATIC_STR) (serror){ .err = STATIC_STR }


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


/**
 * @ingroup PublicAPI
 * @brief Returns `true` if the passed result instance contain `ok` variant.
 * 
 * @param result Instance of any result structure
 * @return true if passed result instance contain `ok` variant
 * @return false if passed result instance contain `err` variant
 */
static inline bool result_is_ok(void* result) {
  if (result) {
    return *((bool*)result);
  } else {
    return false;
  }
}

/**
 * @ingroup PublicAPI
 * @brief Returns `true` if the passed result instance contain `err` variant.
 * 
 * @param result Instance of any result structure
 * @return true if passed result instance contain `err` variant
 * @return false if passed result instance contain `ok` variant
 */
static inline bool result_is_err(void* result) {
  return !result_is_ok(result);
}

/**
 * @ingroup PublicAPI
 * @brief Returns enum thats corresponds result's containing variant
 * @param result Instance of any result structure
 * @return `RES_OK` if passed result instance contain `err` variant
 * @return `RES_ERR` if passed result instance contain `ok` variant
 */
static inline enum result_enum result_match(void* result) {
  if (result_is_ok(result)) {
    return RES_OK;
  } else {
    return RES_ERR;
  }
}


DECLARE_RESULT(int, serror)
DECLARE_RESULT(size_t, serror)
DECLARE_RESULT(char, serror)
DECLARE_RESULT(double, serror)
DECLARE_RESULT(bool, serror)
DECLARE_RESULT(p_void, serror)
DECLARE_RESULT(int_array, serror)
DECLARE_RESULT(double_array, serror)
DECLARE_RESULT(char_array, serror)

