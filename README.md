# qutil.c
Simple utilities library in C23 with no unnecessary overhead and implicit memory allocations.

Some features use compiler extensions! You may supress warnings by defining `R_PEDANTIC_SAFE` before include `qoption.h` or `qresult.h`. If your compiler dont support these exntensions, that feature just not defining in header.

## Result type
```c
typedef enum {
  DIVISION_BY_ZERO,
  INTEGER_OVERFLOW,
  // ...
} arithm_e;

DECLARE_OPTION(arithm_e)
DECLARE_RESULT(double, arithm_e)

struct result_double_arithm_e divide(double val, double devider) {
  if (devider == 0) {
    return result_double_arithm_e_err(DIVISION_BY_ZERO);
  }
  return result_double_arithm_e_ok(val / devider);
}
// ...
struct result_double_arithm_e some_arithmetic() {
  ERROR_PROPAGATE(double, arithm_e);

  auto div = RES_TRY(divide(4, 0));

  auto mul = RES_TRY(multiply(4, 6));
  
  return result_double_arithm_e_ok(div + mul);
}
// ...
int main() {
  auto r = some_arithmetic();
  switch (result_match(&r)) {
    case RES_OK:
      assert(r.is_ok); // for demonstration purposes only! Dont use fields thats starts with underscore!
      printf("ok: %f\n", option_double_unwrap(result_double_arithm_e_get_value(&r))); // result_T_ERR_get_value() returns option_T
      break;
    case RES_ERR: // This branch will be executed
      assert(!r.is_ok);
      printf("error: %d\n", option_arithm_e_unwrap(result_double_arithm_e_get_err(r))); // Or some arithm_e to string function
      break;
  }
}
```
## Option type
```c
// DECLARE_OPTION(int) and DECLARE_OPTION(double) is not required here, they is declared by in qoption.h

struct option_int none_int_opt() {
  return option_int_none();
}

struct option_double opt_tries() {
  OPTION_PROPAGATE(double);
  
  auto i = OPT_TRY(none_int_opt());
  
  return option_double_value(i);
}

struct option_double opt_or_else() {
  return option_double_value(1.0);
}

int main() {
  auto none = opt_tries();
  
  auto v = option_double_or_else(value, opt_or_else);
  assert(v._value == 1.0) // for demonstration purposes only! Dont use fields thats starts with underscore!
}
```

# To do:
- [ ] result type
- - [x] error propagation (TRY macros)
  - [x] result_T_ERR_ok()
  - [x] result_T_ERR_err()
  - [x] result_T_ERR_get_err()
  - [x] result_T_ERR_get_value()
  - [x] result_T_ERR_unwrap()
  - [x] result_T_ERR_unwrap_or()
  - [x] result_T_ERR_inspect()
  - [x] result_T_ERR_inspect_err()
  - [ ] result_T_ERR_and_then()
  - [x] result_T_ERR_or_else()
  - [ ] result_T_ERR_map()
  - [x] result_is_ok()
  - [x] result_ok_err()
  - [x] result_match()
- [ ] option type
- - [x] option propagation (TRY macros)
  - [x] option_T_value()
  - [x] option_T_none()
  - [ ] option_T_and_then()
  - [x] option_T_or_else()
  - [ ] option_T_map()
  - [x] option_T_unwrap()
  - [x] option_T_unwrap_or()
  - [x] option_T_take()
  - [x] option_has_value()
  - [x] option_match()
- [ ] logger
- - [x] Logging levels
  - [ ] Thread safety
  - [x] Formatters
  - [ ] User-defined log destination
  - [ ] Unix compatibility
- [ ] memory
- - [x] arena allocator
  - - [ ] thread safety
    - [ ] version with the ability to allocate new region in case of size exceeds
  - [ ] span type
  - [ ] memory_explicit()

