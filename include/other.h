#pragma once

#include <stddef.h>
[[noreturn]]
void abort_with_error(const char err[static 1]);


// #if __has_c_attribute(__gnu__::__format__)
// 	[[__gnu__::__format__(__printf__, 1, 2)]]
// #endif
[[noreturn]]
void abort_with_error_at(const char msg[static 1], const char file[static 1], size_t line);
