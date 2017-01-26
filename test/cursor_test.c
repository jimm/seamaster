#include "seamaster_test.h"
#include "../src/cursor.h"
#include "cursor_test.h"

void test_cursor_init(patchmaster *pm) {
  song_list *sl = pm->all_songs;
  tassert(cursor_song_list(pm->cursor) == pm->all_songs, "bad song list");

  song *s = list_at(sl->songs, 0);
  tassert(cursor_song(pm->cursor) == s, "bad song");

  patch *p = list_at(s->patches, 0);
  tassert(cursor_patch(pm->cursor) == p, "bad patch");

  test_passed();
}

void test_cursor_next_patch(patchmaster *pm) {
  test_passed();
}

void test_cursor_prev_patch(patchmaster *pm) {
  tassert(true == false, "failed test on purpose");
}

void test_cursor_next_song(patchmaster *pm) {
  test_passed();
}

void test_cursor_prev_song(patchmaster *pm) {
  test_passed();
}

void test_cursor(patchmaster *pm) {
  test_cursor_init(pm);
  test_cursor_next_patch(pm);
  test_cursor_prev_patch(pm);
  test_cursor_next_song(pm);
  test_cursor_prev_song(pm);
}
