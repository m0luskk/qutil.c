#include <check.h>
#include <stdlib.h>

extern Suite* result_option_suite(void);
extern Suite* logger_suite(void);
extern Suite* arena_suite(void);

int main(void) {
  int failed = 0;
  SRunner* runner = nullptr;

  auto result_option = result_option_suite();
  
  runner = srunner_create(result_option);
  
  srunner_run_all(runner, CK_NORMAL);
  
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  // ==--

  auto logger = logger_suite();
  runner = srunner_create(logger);
  
  srunner_run_all(runner, CK_NORMAL);
  
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  // ==--
  
  auto arena = arena_suite();
  runner = srunner_create(arena);
  
  srunner_run_all(runner, CK_NORMAL);
  
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return ( failed ? EXIT_FAILURE : EXIT_SUCCESS  );
}
