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
 
 ## Macros thats uses compilers extensions
 Error propagarion feature (`TRY` macro) use [statement in expression extension](https://gcc.gnu.org/onlinedocs/gcc-12.2.0/gcc/Statement-Exprs.html#Statement-Exprs). 
  Thats cause some warnings of -padantic compilation flag. Define `R_PEDANTIC_SAFE` and `OPTION_PEDANTIC_SAFE` before `result.h` and `option.h` including for toggle of error propagation feature of option and result types;
 */

#include "qoption.h"

/**
 * @brief Contain static error string
 */
typedef static_string serror;

DECLARE_OPTION(serror)

/** @cond */
#define _RESULT_OK_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = true, ._value.ok = value }; }

#define _RESULT_ERR_BODY(T, ERR) { return (struct result_##T##_##ERR){ .is_ok = false, ._value.err = err_value }; }

#define _RESULT_GET_ERR_BODY(T, ERR) { if (result.is_ok) return option_##ERR##_none(); else return option_##ERR##_value(result._value.err); }

#define _RESULT_GET_VALUE_BODY(T, ERR) { if(!result.is_ok) return option_##T##_none(); else return option_##T##_value(result._value.ok); }

#define _RESULT_INSPECT_ARGS(T, ERR) struct result_##T##_##ERR result, f_result_##T##_##ERR##_inspect f
#define _RESULT_INSPECT_BODY(T, ERR) { if (result.is_ok) f(result._value.ok); }

#define _RESULT_INSPECT_ERR_ARGS(T, ERR) struct result_##T##_##ERR result, f_result_##T##_##ERR##_inspect_err f
#define _RESULT_INSPECT_ERR_BODY(T, ERR) { if (!result.is_ok) f(result._value.err); }

#define _RESULT_UNWRAP_BODY(T, ERR) { if (result.is_ok) return result._value.ok; else abort(); }

//#define RESULT_AND_THEN_BODY(T, ERR) { if(!result) abort(); if (result->is_ok) return f(result->_value.ok); else return *result; }
//#define RESULT_AND_THEN_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_and_then f

#define _RESULT_OR_ELSE_ARGS(T, ERR) struct result_##T##_##ERR result, f_result_##T##_##ERR##_or_else f
#define _RESULT_OR_ELSE_BODY(T, ERR) { if (result.is_ok) return result; else return f(result._value.err); }

// R_M(ATTR, RET, NAME, ARGS, BODY)
#define RESULT_METHODS(T, ERR) \
  R_M(_UNSQ_ATTR(), struct result_##T##_##ERR, result_##T##_##ERR##_ok         , T value                          , _RESULT_OK_BODY(T, ERR)) \
  R_M(_UNSQ_ATTR(), struct result_##T##_##ERR, result_##T##_##ERR##_err        , ERR err_value                    , _RESULT_ERR_BODY(T, ERR) ) \
  R_M(_UNSQ_ATTR(), struct option_##ERR      , result_##T##_##ERR##_get_err    , struct result_##T##_##ERR result, _RESULT_GET_ERR_BODY(T, ERR) ) \
  R_M(_UNSQ_ATTR(), struct option_##T        , result_##T##_##ERR##_get_value  , struct result_##T##_##ERR result, _RESULT_GET_VALUE_BODY(T, ERR) ) \
  R_M(_UNSQ_ATTR(), void                     , result_##T##_##ERR##_inspect    , _RESULT_INSPECT_ARGS(T, ERR)      , _RESULT_INSPECT_BODY(T, ERR)) \
  R_M(_UNSQ_ATTR(), void                     , result_##T##_##ERR##_inspect_err, _RESULT_INSPECT_ERR_ARGS(T, ERR)  , _RESULT_INSPECT_ERR_BODY(T, ERR)) \
  R_M(              , T                        , result_##T##_##ERR##_unwrap     , struct result_##T##_##ERR result, _RESULT_UNWRAP_BODY(T, ERR) ) \
  R_M(              , struct result_##T##_##ERR, result_##T##_##ERR##_or_else    , _RESULT_OR_ELSE_ARGS(T, ERR)      , _RESULT_OR_ELSE_BODY(T, ERR) )

#define R_M(ATTR, RET, NAME, ARGS, DEF) [[maybe_unused]] ATTR static inline RET NAME(ARGS) DEF
/** @endcond */

/**
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
typedef struct result_##T##_##ERR(*f_result_##T##_##ERR##_or_else)(ERR); \
RESULT_METHODS(T, ERR)


#if defined(__GNUC__) || defined(__clang__)
#ifdef R_PEDANTIC_SAFE
  #define EXTENSION_ATTR __extension__
#else
  #define EXTENSION_ATTR
#endif
/**
 * @brief Defines function poiner thats using in `TRY` macro.
 */

  #define ERROR_PROPAGATE(T, ERR) [[maybe_unused]] struct result_##T##_##ERR(*const __f_res_ret_err)(ERR err) = result_##T##_##ERR##_err;
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
#define RES_TRY(expr) EXTENSION_ATTR ({ \
    auto _tmp = (expr); \
    if (!result_is_ok(&_tmp)) { \
        return __f_res_ret_err(_tmp._value.err); \
    } \
    _tmp._value.ok; \
})
#endif

/**
 * @brief Return type of `result_match()` function
 * 
 */
enum result_enum {
  RES_OK,
  RES_ERR,
};


/**
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




