#pragma once

/**
 * @file option.h
 * @brief Option type. 
 * @ingroup PublicAPI
 * Result is a type that represents either value or nothing.
 */

#define DECLARE_OPTION(T) \
struct option_##T { \
  bool has_value; \
  T _value; \
}; \
struct option_##T option_##T##_value(T val); \
struct option_##T option_##T##_none();

#define IMPL_OPTION(T) \
struct option_##T option_##T##_value(T val) { \
  return (struct option_##T){.has_value = true, ._value = val}; \
} \
struct option_##T option_##T##_none() { \
  return (struct option_##T){.has_value = false}; \
}

enum option_enum {
  OPT_VALUE,
  OPT_NONE
};

enum option_enum option_match(void* option);

bool option_has_value(void* option);

DECLARE_OPTION(int)

