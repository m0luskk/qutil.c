#ifndef _Q_OPTION_H
#define _Q_OPTION_H

/**
 * @file option.h
 * @brief Option type. 
 * @ingroup PublicAPI
 * Result is a type that represents either value or nothing.
 */

#include "qother.h"
#include <string.h>

/** @cond */
#define _Q_OPTION_VALUE_BODY(T) { return (struct q_option_##T){.has_value = true, ._value = val}; }

#define _Q_OPTION_NONE_BODY(T) { return (struct q_option_##T){.has_value = false}; }

#define _Q_OPTION_UNWRAP_BODY(T) { if (opt.has_value) return opt._value; else _Q_ABORT_HERE(); }

#define _Q_OPTION_UNWRAP_OR_ARGS(T) const struct q_option_##T opt, T def
#define _Q_OPTION_UNWRAP_OR_BODY(T) { if (opt.has_value) return opt._value; else return def; }

#define _Q_OPTION_TAKE_BODY(T) { \
  if (!opt) _Q_ABORT_HERE(); \
  if (opt->has_value) { \
    auto tmp = q_option_##T##_value(opt->_value); \
    opt->has_value = false; \
    memset(&(opt->_value), 0, sizeof(T)); /* TODO: memset_explicit */ \
    return tmp; \
  } else { \
    return q_option_##T##_none(); \
  } \
}

#define _Q_OPTION_OR_ELSE_ARGS(T) const struct q_option_##T opt, q_option_##T##_or_else_f f
#define _Q_OPTION_OR_ELSE_BODY(T) { if(opt.has_value) return opt; else return f(); } 

#define _Q_OPTION_METHODS(T) \
  O_M(Q_UNSQ_ATTR(), struct q_option_##T, q_option_##T##_value    , T val                             , _Q_OPTION_VALUE_BODY(T) )    \
  O_M(Q_UNSQ_ATTR(), struct q_option_##T, q_option_##T##_none     ,                                   , _Q_OPTION_NONE_BODY(T)  )    \
  O_M(              , T                  , q_option_##T##_unwrap   , const struct q_option_##T opt     , _Q_OPTION_UNWRAP_BODY(T))    \
  O_M(Q_UNSQ_ATTR(), T                  , q_option_##T##_unwrap_or, _Q_OPTION_UNWRAP_OR_ARGS(T)       , _Q_OPTION_UNWRAP_OR_BODY(T)) \
  O_M(              , struct q_option_##T, q_option_##T##_take     , struct q_option_##T* opt , _Q_OPTION_TAKE_BODY(T))      \
  O_M(              , struct q_option_##T, q_option_##T##_or_else  , _Q_OPTION_OR_ELSE_ARGS(T)         , _Q_OPTION_OR_ELSE_BODY(T))

#define O_M(ATTR, RET, NAME, ARGS, BODY) [[maybe_unused]] ATTR static inline RET NAME(ARGS) BODY
/** @endcond */

#define Q_OPTION(T) q_option_##T##_t

#define Q_DECLARE_OPTION(T) \
typedef struct q_option_##T { \
  bool has_value; /* TODO: const fields (with worked non-ub take() method) */ \
  T _value; \
} q_option_##T##_t; \
typedef struct q_option_##T(*q_option_##T##_or_else_f)(); \
_Q_OPTION_METHODS(T)

#if defined(__GNUC__) || defined(__clang__)
#ifndef _Q_EXTENSION_ATTR
  #ifdef Q_PEDANTIC_SAFE
    #define _Q_EXTENSION_ATTR __extension__
  #else
    #define _Q_EXTENSION_ATTR
  #endif
#endif
/**
 * @brief Defines function poiner thats using in `TRY` macro.
 */
#define Q_OPTION_CONTEXT(T) \
[[maybe_unused]] struct q_option_##T(*const __f_opt_ret_none)() = q_option_##T##_none;   \
[[maybe_unused]] struct q_option_##T(*const __f_opt_ret_value)(T) = q_option_##T##_value; \

#define Q_OPTION_NONE() __f_opt_ret_none()
#define Q_OPTION_VALUE(...) __f_opt_ret_value(__VA_ARGS__)

#define Q_NONE() __f_opt_ret_none()
#define Q_VALUE(...) __f_opt_ret_value(__VA_ARGS__)

#define Q_OPT_HAS_VALUE(OPT) (OPT)->has_value

/**
 * @brief If `expr` is the option type and contain the `none` variant, then propagates it from current function. Otherwise returns `value`
 * @attention Before using OPT_TRY macros you must use `NONE_PROPAGATE` macros above with `T` of option type of function

 ```c
NONE_PROPAGATE(double);
  
auto i = Q_OPT_TRY(none_int_opt()); // will propagate `none`
  
return q_option_double_value(i);
 ```
 */
#define Q_OPT_TRY(EXPR) _Q_EXTENSION_ATTR ({ \
  auto __tmp = (EXPR); \
  if (!Q_OPT_HAS_VALUE(&__tmp)) { \
      return __f_opt_ret_none(); \
  } \
  __tmp._value; \
})

#define Q_OPT_UNWRAP(EXPR) _Q_EXTENSION_ATTR ({ \
  auto __tmp = (EXPR); \
  if (!Q_OPT_HAS_VALUE(&__tmp)) { \
    _Q_ABORT_HERE(); \
  } \
  __tmp._value; \
})

#define Q_OPT_UNWRAP_REF(EXPR) _Q_EXTENSION_ATTR ({ \
  if (!Q_OPT_HAS_VALUE(EXPR)) { _Q_ABORT_HERE(); } \
  &((EXPR)->_value); \
})

#endif

typedef enum q_option_enum {
  OPT_VALUE,
  OPT_NONE
} q_option_enum_t;

static inline bool q_option_has_value(void* option) {
  if (option) {
    return *((bool*)option);
  } else {
    return false;
  }
}

static inline enum q_option_enum q_option_match(void* option) {
  if (q_option_has_value(option)) {
    return OPT_VALUE;
  } else {
    return OPT_NONE;
  }
}


Q_DECLARE_OPTION(int)
Q_DECLARE_OPTION(size_t)
Q_DECLARE_OPTION(char)
Q_DECLARE_OPTION(double)
Q_DECLARE_OPTION(bool)
Q_DECLARE_OPTION(void_p)
Q_DECLARE_OPTION(int_array)
Q_DECLARE_OPTION(double_array)
Q_DECLARE_OPTION(char_array)
Q_DECLARE_OPTION(cstring)


#endif
