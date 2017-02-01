#include <stdlib.h>
#include "test_helper.h"
#include "../src/list.h"
#include "list_test.h"

void test_list_slice() {
  List list, *l;
  list << (void *)"a";
  list << (void *)"b";
  list << (void *)"c";

  l = list.slice(0, -1);
  tassert(l->length() == 3, 0);
  tassert(*(char *)l->at(0) == 'a', 0);
  tassert(*(char *)l->at(1) == 'b', 0);
  tassert(*(char *)l->at(2) == 'c', 0);
  delete l;

  l = list.slice(0, 0);
  tassert(l->length() == 0, 0);
  delete l;

  l = list.slice(1, 2);
  tassert(l->length() == 2, 0);
  tassert(*(char *)l->at(0) == 'b', 0);
  tassert(*(char *)l->at(1) == 'c', 0);
  delete l;

  l = list.slice(0, 99);
  tassert(l->length() == 3, 0);
  tassert(*(char *)l->at(2) == 'c', 0);
  delete l;
}

void test_list_join() {
  List list;

  char *joined = list.join(", ");
  tassert(strlen(joined) == 0, 0);
  free(joined);

  list << (void *)"a";
  list << (void *)"b";
  list << (void *)"c";
  joined = list.join(", ");
  tassert(strcmp(joined, "a, b, c") == 0, 0);
  free(joined);
}

void test_list() {
  test_run(test_list_slice);
  test_run(test_list_join);
}
