#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "seamaster_test.h"
#include "../src/loader.h"
#include "../src/cursor.h"
#include "formatter_test.h"
#include "loader_test.h"
#include "cursor_test.h"
#include "controller_test.h"
#include "connection_test.h"
#include "input_test.h"

// ================ running tests ================

test_results results;

void test_results_init() {
  results.num_tests = results.num_errors = 0;
}

void test_passed() {
  ++results.num_tests;
  printf(".");
}

void test_failed() {
  ++results.num_tests;
  ++results.num_errors;
  printf("*");
}

int test_num_errors() {
  return results.num_errors;
}

void print_time() {
  rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  printf("\n\nFinished in %ld.%06ld seconds\n", usage.ru_utime.tv_sec,
         (long)usage.ru_utime.tv_usec);
}

void print_results() {
  printf("\nTests run: %d, tests passed: %d, tests failed: %d\n",
         results.num_tests, results.num_tests - results.num_errors,
         results.num_errors);
  printf("done\n");
}

void run_tests() {
  test_formatter();
  test_load();
  test_cursor();
  test_controller();
  test_connection();
  test_input();
}

void run_tests_and_print_results() {
  run_tests();
  print_time();
  print_results();
}

// ================ main ================

int main(int argc, const char **argv) {
  run_tests_and_print_results();
  exit(results.num_errors == 0 ? 0 : 1);
  return 0;
}
