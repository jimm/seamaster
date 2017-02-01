#include <stdio.h>
#include <stdlib.h>
#include "seamaster_test.h"
#include "../src/loader.h"
#include "../src/cursor.h"
#include "list_test.h"
#include "loader_test.h"
#include "cursor_test.h"
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

void run_tests() {
  test_list();
  test_load();
  test_cursor();
  test_connection();
  test_input();
  printf("\n\nTests run: %d, tests passed: %d, tests failed: %d\n",
         results.num_tests, results.num_tests - results.num_errors,
         results.num_errors);
  printf("done\n");
}

// ================ main ================

int main(int argc, const char **argv) {
  run_tests();
  exit(results.num_errors == 0 ? 0 : 1);
  return 0;
}
