#include <stdio.h>
#include <stdlib.h>
#include "seamaster_test.h"
#include "../src/load.h"
#include "../src/cursor.h"
#include "load_test.h"
#include "cursor_test.h"
#include "connection_test.h"

#define TEST_FILE "test/testfile.sm"

// ================ running tests ================

test_results results;

void test_start(PatchMaster *pm) {
  pm->cursor->init();
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

void test_clear_midi(PatchMaster *pm) {
  for (int i = 0; i < list_length(pm->inputs); ++i) {
    Input *in = (Input *)list_at(pm->inputs, i);
    in->clear();
  }
  for (int i = 0; i < list_length(pm->outputs); ++i) {
    Output *out = (Output *)list_at(pm->outputs, i);
    out->clear();
  }
}

void run_tests(PatchMaster *pm) {
  test_load(pm);
  test_cursor(pm);
  test_connection(pm);
  printf("\n\nTests run: %d, tests passed: %d, tests failed: %d\n",
         results.num_tests, results.num_tests - results.num_errors,
         results.num_errors);
  printf("done\n");
}

// ================ main ================

int main(int argc, const char **argv) {
  PatchMaster *pm = new PatchMaster();
  pm->testing = true;
  fprintf(stderr, "loading %s\n", TEST_FILE); /* DEBUG */
  if (load(pm, TEST_FILE) != 0)
      exit(1);                /* error already printed */

  pm->start();
  run_tests(pm);
  pm->stop();
  delete pm;

  exit(results.num_errors == 0 ? 0 : 1);
  return 0;
}
