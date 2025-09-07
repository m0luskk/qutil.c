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
 * ## Using user-defined types
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
 * \attention `user_ok_type` and `user_err_type` must not contain whitespaces and special characters (`const char *` or `int[]` for example)
 
 */

#include "option.h"

/**
 * @brief Contain static error string
 */
typedef static_string serror;

DECLARE_OPTION(serror)

/** @cond */
#if __has_attribute(unsequenced)
  #define UNSEQUENCED_ATTR() [[unsequenced]]
#else
  #define UNSEQUENCED_ATTR()
#endif

#define RESULT_OK_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = true, ._value.ok = value }; }

#define RESULT_ERR_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = false, ._value.err = err_value }; }

#define RESULT_GET_ERR_BODY(T, ERR) { if(!result) return option_##ERR##_none(); else return (result->is_ok ? option_##ERR##_none() : option_##ERR##_value(result->_value.err)); }

#define RESULT_GET_VALUE_BODY(T, ERR) { if(!result) return option_##T##_none(); else return (!result->is_ok ? option_##T##_none() : option_##T##_value(result->_value.ok)); }

#define RESULT_INSPECT_BODY(T, ERR) { if(!result) return;  if (result->is_ok) f(result->_value.ok); }
#define RESULT_INSPECT_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_inspect f

#define RESULT_INSPECT_ERR_BODY(T, ERR) { if(!result) return; if (!result->is_ok) f(result->_value.err); }
#define RESULT_INSPECT_ERR_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_inspect_err f

// R_M(ATTR, RET, NAME, ARGS, BODY)
#define RESULT_METHODS(T, ERR) \
  R_M(, struct result_##T##_##ERR, result_##T##_##ERR##_ok, T value, RESULT_OK_BODY(T, ERR)) \
  R_M(, struct result_##T##_##ERR, result_##T##_##ERR##_err, ERR err_value, RESULT_ERR_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), struct option_##ERR, result_##T##_##ERR##_get_err, struct result_##T##_##ERR* result, RESULT_GET_ERR_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), struct option_##T, result_##T##_##ERR##_get_value, struct result_##T##_##ERR* result, RESULT_GET_VALUE_BODY(T, ERR) ) \
  R_M(UNSEQUENCED_ATTR(), void, result_##T##_##ERR##_inspect, RESULT_INSPECT_ARGS(T, ERR), RESULT_INSPECT_BODY(T, ERR)) \
  R_M(UNSEQUENCED_ATTR(), void, result_##T##_##ERR##_inspect_err, RESULT_INSPECT_ERR_ARGS(T, ERR), RESULT_INSPECT_ERR_BODY(T, ERR))

#define R_M(ATTR, RET, NAME, ARGS, DEF) ATTR static inline RET NAME(ARGS) DEF
/** @endcond */

/**
 * @ingroup PublicAPI
 * @brief Declare result structure of types `T` as `ok` variant and `ERR` as `err` variant.
 */
#define DECLARE_RESULT(T, ERR) \
struct result_##T##_##ERR { \
  const bool is_ok; \
  const union { \
    T ok; \
    ERR err; \
  } _value; \
}; \
typedef void(*f_result_##T##_##ERR##_inspect_err)(ERR); \
typedef void(*f_result_##T##_##ERR##_inspect)(T); \
RESULT_METHODS(T, ERR)

// #define TRY(T, ERR, RESULT) if(!RESULT.is_ok) return result_##T##_##ERR##_err(RESULT._value.err);
#if defined(__GNUC__) || defined(__clang__)
#ifndef RESULT_PEDANTIC_SAFE
/**
 * @brief Defines function poiner thats using in `TRY` macro.
 */
#define ERROR_PROPAGATE(T, ERR) struct result_##T##_##ERR(*__f_ret_err)(ERR err) = result_##T##_##ERR##_err

/**
 * @brief If `expr` is the result type and contain the `err` variant, then propagates it from current function. Otherwise return `ok` variant value
 * @attention Before using TRY macros user must use ERROR_PROPAGATE macros with `T` and `ERR` of result type of function

 ```c
struct result_double_serror foo() {
  ERROR_PROPAGATE(double, serror);

  auto res = TRY(devide(4, 0));

  auto ptr = TRY(another_fail_function());

  return result_double_serror_ok(res);
}
 ```
 */
#define TRY(expr) ({ \
    auto _tmp = (expr); \
    if (!result_is_ok(&_tmp)) { \
        return __f_ret_err(_tmp._value.err); \
    } \
    _tmp._value.ok; \
})
#endif
#endif

/**
 * @brief Return type of `result_match` function
 * 
 */
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

