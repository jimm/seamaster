#ifndef SEAMASTER_TEST_H
#define SEAMASTER_TEST_H

#include "../src/patchmaster.h"

typedef struct test_results {
  int num_tests;
  int num_errors;
} test_results;

void test_results_init();
void test_passed();
void test_failed();

void test_clear(patchmaster *pm);

#endif /* SEAMASTER_TEST_H */