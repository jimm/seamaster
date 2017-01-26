#ifndef SEAMASTER_TEST_H
#define SEAMASTER_TEST_H

#include <stdio.h>
#include "../src/patchmaster.h"

typedef struct test_results {
  int num_tests;
  int num_errors;
} test_results;

void test_results_init();
void test_passed();
void test_failed();

#define tassert(test, errmsg)                                           \
  {                                                                     \
    if (!(test)) {                                                      \
      printf("\n%s:%d:0: error: %s\n", __FILE__, __LINE__, errmsg ? errmsg : "test failed"); \
      test_failed();                                                    \
      return;                                                           \
    }                                                                   \
  }

void test_clear_midi(patchmaster *pm);

#endif /* SEAMASTER_TEST_H */
