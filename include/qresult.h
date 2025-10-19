#ifndef _Q_RESULT_H
#define _Q_RESULT_H

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
typedef cstring serror;

Q_DECLARE_OPTION(serror)

/** @cond */
#define _Q_RESULT_OK_BODY(T, ERR) { return (struct q_result_##T##_##ERR){ .is_ok = true, ._value.ok = value }; }

#define _Q_RESULT_ERR_BODY(T, ERR) { return (struct q_result_##T##_##ERR){ .is_ok = false, ._value.err = err_value }; }

#define _Q_RESULT_GET_ERR_BODY(T, ERR) { if (result.is_ok) return q_option_##ERR##_none(); else return q_option_##ERR##_value(result._value.err); }

#define _Q_RESULT_GET_VALUE_BODY(T, ERR) { if(!result.is_ok) return q_option_##T##_none(); else return q_option_##T##_value(result._value.ok); }

#define _Q_RESULT_INSPECT_ARGS(T, ERR) struct q_result_##T##_##ERR result, q_result_##T##_##ERR##_inspect_f f
#define _Q_RESULT_INSPECT_BODY(T, ERR) { if (result.is_ok) f(result._value.ok); }

#define _Q_RESULT_INSPECT_ERR_ARGS(T, ERR) struct q_result_##T##_##ERR result, q_result_##T##_##ERR##_inspect_err_f f
#define _Q_RESULT_INSPECT_ERR_BODY(T, ERR) { if (!result.is_ok) f(result._value.err); }

#define _Q_RESULT_UNWRAP_BODY(T, ERR) { if (result.is_ok) return result._value.ok; else abort(); }

//#define RESULT_AND_THEN_BODY(T, ERR) { if(!result) abort(); if (result->is_ok) return f(result->_value.ok); else return *result; }
//#define RESULT_AND_THEN_ARGS(T, ERR) struct result_##T##_##ERR* result, f_result_##T##_##ERR##_and_then f

#define _Q_RESULT_OR_ELSE_ARGS(T, ERR) struct q_result_##T##_##ERR result, q_result_##T##_##ERR##_or_else_f f
#define _Q_RESULT_OR_ELSE_BODY(T, ERR) { if (result.is_ok) return result; else return f(result._value.err); }

// R_M(ATTR, RET, NAME, ARGS, BODY)
#define _Q_RESULT_METHODS(T, ERR) \
  R_M(Q_UNSQ_ATTR(), struct q_result_##T##_##ERR , q_result_##T##_##ERR##_ok         , T value                             , _Q_RESULT_OK_BODY(T, ERR)) \
  R_M(Q_UNSQ_ATTR(), struct q_result_##T##_##ERR , q_result_##T##_##ERR##_err        , ERR err_value                       , _Q_RESULT_ERR_BODY(T, ERR) ) \
  R_M(Q_UNSQ_ATTR(), struct q_option_##ERR       , q_result_##T##_##ERR##_get_err    , struct q_result_##T##_##ERR result  , _Q_RESULT_GET_ERR_BODY(T, ERR) ) \
  R_M(Q_UNSQ_ATTR(), struct q_option_##T         , q_result_##T##_##ERR##_get_value  , struct q_result_##T##_##ERR result  , _Q_RESULT_GET_VALUE_BODY(T, ERR) ) \
  R_M(Q_UNSQ_ATTR(), void                        , q_result_##T##_##ERR##_inspect    , _Q_RESULT_INSPECT_ARGS(T, ERR)      , _Q_RESULT_INSPECT_BODY(T, ERR)) \
  R_M(Q_UNSQ_ATTR(), void                        , q_result_##T##_##ERR##_inspect_err, _Q_RESULT_INSPECT_ERR_ARGS(T, ERR)  , _Q_RESULT_INSPECT_ERR_BODY(T, ERR)) \
  R_M(              , T                           , q_result_##T##_##ERR##_unwrap     , struct q_result_##T##_##ERR result  , _Q_RESULT_UNWRAP_BODY(T, ERR) ) \
  R_M(              , struct q_result_##T##_##ERR , q_result_##T##_##ERR##_or_else    , _Q_RESULT_OR_ELSE_ARGS(T, ERR)      , _Q_RESULT_OR_ELSE_BODY(T, ERR) )

#define R_M(ATTR, RET, NAME, ARGS, DEF) [[maybe_unused]] ATTR static inline RET NAME(ARGS) DEF
/** @endcond */

/**
 * @brief Declare result structure of types `T` as `ok` variant and `ERR` as `err` variant.
 */
#define Q_DECLARE_RESULT(T, ERR) \
typedef struct q_result_##T##_##ERR { \
  const bool is_ok; \
  union { \
    T ok; \
    ERR err; \
  } _value; \
} q_result_##T##_##ERR##_t; \
typedef void(*q_result_##T##_##ERR##_inspect_err_f)(ERR); \
typedef void(*q_result_##T##_##ERR##_inspect_f)(T); \
typedef struct q_result_##T##_##ERR(*q_result_##T##_##ERR##_or_else_f)(ERR); \
_Q_RESULT_METHODS(T, ERR)


#if defined(__GNUC__) || defined(__clang__)
#ifdef Q_PEDANTIC_SAFE
  #define _Q_EXTENSION_ATTR __extension__
#else
  #define _Q_EXTENSION_ATTR
#endif
/**
 * @brief Defines function poiner thats using in `TRY` macro.
 */

#define Q_RESULT_CONTEXT(T, ERR) \
[[maybe_unused]] struct q_result_##T##_##ERR(*const __f_res_ret_err)(ERR err) = q_result_##T##_##ERR##_err; \
[[maybe_unused]] struct q_result_##T##_##ERR(*const __f_res_ret_ok)(T ok) = q_result_##T##_##ERR##_ok; \

#define Q_RES_IS_OK(EXPR) (EXPR)->is_ok

#define Q_RES_ERR(...) __f_res_ret_err(__VA_ARGS__)
#define Q_RES_OK(...) __f_res_ret_ok(__VA_ARGS__)
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
#define Q_RES_TRY(EXPR) _Q_EXTENSION_ATTR ({ \
    auto _tmp = (EXPR); \
    if (!Q_RES_IS_OK(&_tmp)) { \
        return __f_res_ret_err(_tmp._value.err); \
    } \
    _tmp._value.ok; \
})
#define Q_RES_UNWRAP(EXPR) _Q_EXTENSION_ATTR ({ \
  auto _tmp = (EXPR); \
  if (!Q_RES_IS_OK(&_tmp)) abort(); \
  _tmp._value.ok; \
})

// EXPR is a pointer
#define Q_RES_UNWRAP_MUT(EXPR) _Q_EXTENSION_ATTR ({ \
  if (!Q_RES_IS_OK(EXPR)) { abort(); } \
  &((EXPR)->_value.ok); \
})

#endif


/**
 * @brief Return type of `result_match()` function
 * 
 */
typedef enum q_result_enum {
  RES_OK,
  RES_ERR,
} q_result_enum_t;


/**
 * @brief Returns `true` if the passed result instance contain `ok` variant.
 * 
 * @param result Instance of any result structure
 * @return true if passed result instance contain `ok` variant
 * @return false if passed result instance contain `err` variant
 */
static inline bool q_result_is_ok(void* result) {
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
static inline bool q_result_is_err(void* result) {
  return !q_result_is_ok(result);
}

/**
 * @brief Returns enum thats corresponds result's containing variant
 * @param result Instance of any result structure
 * @return `RES_OK` if passed result instance contain `err` variant
 * @return `RES_ERR` if passed result instance contain `ok` variant
 */
static inline enum q_result_enum q_result_match(void* result) {
  if (q_result_is_ok(result)) {
    return RES_OK;
  } else {
    return RES_ERR;
  }
}


Q_DECLARE_RESULT(int, serror)
Q_DECLARE_RESULT(size_t, serror)
Q_DECLARE_RESULT(char, serror)
Q_DECLARE_RESULT(double, serror)
Q_DECLARE_RESULT(bool, serror)
Q_DECLARE_RESULT(void_p, serror)
Q_DECLARE_RESULT(int_array, serror)
Q_DECLARE_RESULT(double_array, serror)
Q_DECLARE_RESULT(char_array, serror)


#endif

