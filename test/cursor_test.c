#include "seamaster_test.h"
#include "cursor_test.h"

void test_cursor_init() {
  test_passed();
}

void test_cursor_next_patch() {
  test_passed();
}

void test_cursor_prev_patch() {
  test_failed();
}

void test_cursor_next_song() {
  test_passed();
}

void test_cursor_prev_song() {
  test_passed();
}

void test_cursor(patchmaster *pm) {
  test_cursor_init();
  test_cursor_next_patch();
  test_cursor_prev_patch();
  test_cursor_next_song();
  test_cursor_prev_song();
}
