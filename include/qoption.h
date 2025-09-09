#pragma once

/**
 * @file option.h
 * @brief Option type. 
 * @ingroup PublicAPI
 * Result is a type that represents either value or nothing.
 */

#include "qother.h"
#include <string.h>

/** @cond */
#define OPTION_VALUE_BODY(T) { return (struct option_##T){.has_value = true, ._value = val}; }

#define OPTION_NONE_BODY(T) { return (struct option_##T){.has_value = false}; }

#define OPTION_UNWRAP_BODY(T) { if (!opt) abort(); if (opt->has_value) return opt->_value; else abort(); }

#define OPTION_UNWRAP_OR_ARGS(T) const struct option_##T* opt, T def
#define OPTION_UNWRAP_OR_BODY(T) { if (!opt) abort(); if (opt->has_value) return opt->_value; else return def; }

#define OPTION_TAKE_BODY(T) { \
  if (!opt) abort(); \
  if (opt->has_value) { \
    auto tmp = option_##T##_value(opt->_value); \
    opt->has_value = false; \
    memset(&(opt->_value), 0, sizeof(T)); /* TODO: memset_explicit */ \
    return tmp; \
  } else { \
    return option_##T##_none(); \
  } \
}

#define OPTION_OR_ELSE_ARGS(T) const struct option_##T* opt, f_option_##T##_or_else f
#define OPTION_OR_ELSE_BODY(T) { if(!opt) abort(); if(opt->has_value) return *opt; else return f(); } 

#define OPTION_METHODS(T) \
  O_M(struct option_##T, option_##T##_value    , T val                         , OPTION_VALUE_BODY(T) ) \
  O_M(struct option_##T, option_##T##_none     ,                               , OPTION_NONE_BODY(T)  ) \
  O_M(T                , option_##T##_unwrap   , const struct option_##T* opt  , OPTION_UNWRAP_BODY(T)) \
  O_M(T                , option_##T##_unwrap_or, OPTION_UNWRAP_OR_ARGS(T)      , OPTION_UNWRAP_OR_BODY(T)) \
  O_M(struct option_##T, option_##T##_take     , struct option_##T* opt        , OPTION_TAKE_BODY(T)) \
  O_M(struct option_##T, option_##T##_or_else  , OPTION_OR_ELSE_ARGS(T)        , OPTION_OR_ELSE_BODY(T))

#define O_M(RET, NAME, ARGS, BODY) static inline RET NAME(ARGS) BODY
/** @endcond */

#define DECLARE_OPTION(T) \
struct option_##T { \
  bool has_value; /* TODO: const fields (with worked non-ub take() method) */ \
  T _value; \
}; \
typedef struct option_##T(*f_option_##T##_or_else)(); \
OPTION_METHODS(T)

#if defined(__GNUC__) || defined(__clang__)
#ifdef R_PEDANTIC_SAFE
  #define EXTENSION_ATTR __extension__
#else
  #define EXTENSION_ATTR
#endif
/**
 * @brief Defines function poiner thats using in `TRY` macro.
 */
#define NONE_PROPAGATE(T) [[maybe_unused]] struct option_##T(*const __f_opt_ret_none)() = option_##T##_none;

/**
 * @brief If `expr` is the option type and contain the `none` variant, then propagates it from current function. Otherwise returns `value`
 * @attention Before using OPT_TRY macros you must use `NONE_PROPAGATE` macros above with `T` of option type of function

 ```c
NONE_PROPAGATE(double);
  
auto i = OPT_TRY(none_int_opt()); // will propagate `none`
  
return option_double_value(i);
 ```
 */
#define OPT_TRY(expr) EXTENSION_ATTR ({ \
    auto _tmp = (expr); \
    if (!option_has_value(&_tmp)) { \
        return __f_opt_ret_none(); \
    } \
    _tmp._value; \
})
#endif

enum option_enum {
  OPT_VALUE,
  OPT_NONE
};

static inline bool option_has_value(void* option) {
  if (option) {
    return *((bool*)option);
  } else {
    return false;
  }
}

static inline enum option_enum option_match(void* option) {
  if (option_has_value(option)) {
    return OPT_VALUE;
  } else {
    return OPT_NONE;
  }
}


DECLARE_OPTION(int)
DECLARE_OPTION(size_t)
DECLARE_OPTION(char)
DECLARE_OPTION(double)
DECLARE_OPTION(bool)
DECLARE_OPTION(p_void)
DECLARE_OPTION(int_array)
DECLARE_OPTION(double_array)
DECLARE_OPTION(char_array)
DECLARE_OPTION(static_string)

