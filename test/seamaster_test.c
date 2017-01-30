#include <stdio.h>
#include <stdlib.h>
#include "seamaster_test.h"
#include "../src/load.h"
#include "../src/cursor.h"
#include "load_test.h"
#include "cursor_test.h"

#define TEST_FILE "test/testfile.sm"

// ================ running tests ================

test_results results;

void test_start(patchmaster *pm) {
  cursor_init(pm->cursor);
  test_clear_midi(pm);
}

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

void test_clear_midi(patchmaster *pm) {
  for (int i = 0; i < list_length(pm->inputs); ++i) {
    input *in = list_at(pm->inputs, i);
    input_clear(in);
  }
  for (int i = 0; i < list_length(pm->outputs); ++i) {
    output *out = list_at(pm->outputs, i);
    output_clear(out);
  }
}

void run_tests(patchmaster *pm) {
  // TODO
  test_load(pm);
  test_cursor(pm);
  // TODO
  /* test_connection(pm); */
  printf("\n\nTests run: %d, tests passed: %d, tests failed: %d\n",
         results.num_tests, results.num_tests - results.num_errors,
         results.num_errors);
  printf("done\n");
}

// ================ main ================

int main(int argc, const char **argv) {
  patchmaster *pm = patchmaster_new();
  pm->testing = true;
  fprintf(stderr, "loading %s\n", TEST_FILE); /* DEBUG */
  if (load(pm, TEST_FILE) != 0)
      exit(1);                /* error already printed */

  patchmaster_start(pm);
  run_tests(pm);
  patchmaster_stop(pm);
  patchmaster_free(pm);

  exit(results.num_errors == 0 ? 0 : 1);
  return 0;
}
