# qutil.c
Simple utility library in C23

# To do:
<details>
<summary>std functionality</summary>
  
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
- - [ ] Logging levels
  - [ ] Thread-safety
  - [ ] Formatters
  - [ ] User-defined log destination
  - [ ] Unix compatibility
- [ ] arena allocator

</details>
