#include <stdlib.h>
#include "test_helper.h"
#include "../src/list.h"
#include "list_test.h"

void test_list_index_of() {
  List<int> list;
  list << 10;
  list << 20;
  list << 30;

  tassert(list.index_of(10) == 0, 0);
  tassert(list.index_of(30) == 2, 0);
  tassert(list.index_of(42) == -1, 0);
}

void test_list_slice() {
  List<int> list, *l;
  list << 10;
  list << 20;
  list << 30;

  l = list.slice(0, -1);
  tassert(l->length() == 3, 0);
  tassert(l->at(0) == 10, 0);
  tassert(l->at(1) == 20, 0);
  tassert(l->at(2) == 30, 0);
  delete l;

  l = list.slice(0, 0);
  tassert(l->length() == 0, 0);
  delete l;

  l = list.slice(1, 2);
  tassert(l->length() == 2, 0);
  tassert(l->at(0) == 20, 0);
  tassert(l->at(1) == 30, 0);
  delete l;

  l = list.slice(0, 99);
  tassert(l->length() == 3, 0);
  tassert(l->at(2) == 30, 0);
  delete l;
}

void test_list_join() {
  List<char *> list;

  char *joined = list.join(", ");
  tassert(strlen(joined) == 0, 0);
  free(joined);

  list << (char *)"a";
  list << (char *)"b";
  list << (char *)"c";
  joined = list.join(", ");
  tassert(strcmp(joined, "a, b, c") == 0, 0);
  free(joined);
}

void test_list() {
  test_run(test_list_index_of);
  test_run(test_list_slice);
  test_run(test_list_join);
}
