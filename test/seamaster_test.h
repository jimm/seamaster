#ifndef SEAMASTER_TEST_H
#define SEAMASTER_TEST_H

#include <stdio.h>
#include "../src/patchmaster.h"

typedef struct test_results {
  int num_tests;
  int num_errors;
} test_results;

void test_start(patchmaster *pm);
void test_results_init();
void test_passed();
void test_failed();
int test_num_errors();

// assumes "pm" is defined
#define test_run(name)                          \
  {                                             \
    int num_errors = test_num_errors();         \
    test_start(pm);                             \
    name(pm);                                   \
    if (test_num_errors() == num_errors)        \
      test_passed();                            \
  }

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
