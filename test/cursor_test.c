#include "seamaster_test.h"
#include "../src/cursor.h"
#include "cursor_test.h"

void test_cursor_init_empty(patchmaster *_) {
  patchmaster *pm = patchmaster_new();
  test_start(pm);
  cursor *c = pm->cursor;
  cursor_init(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == -1, "bad song");
  tassert(c->patch_index == -1, "bad patch");
  test_passed();
}

void test_cursor_init(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_next_patch(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 1, "bad patch");
  test_passed();
}

void test_cursor_next_patch_at_end_of_song(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  c->patch_index = 1;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 1, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_next_patch_at_end_of_song_list(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 1, "bad song");
  tassert(c->patch_index == 1, "bad patch");
  test_passed();
}

void test_cursor_prev_patch(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  c->patch_index = 1;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_prev_patch_start_of_song(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  c->song_index = 1;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_prev_patch_start_of_song_list(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_next_song(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  cursor_next_song(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 1, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_prev_song(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  cursor_prev_song(c);
  tassert(c->song_list_index == 0, "bad song list");
  tassert(c->song_index == 0, "bad song");
  tassert(c->patch_index == 0, "bad patch");
  test_passed();
}

void test_cursor_song_list(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  tassert(cursor_song_list(c) == pm->all_songs, "bad song list");
  test_passed();
}

void test_cursor_song(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  tassert(cursor_song(c) == list_at(pm->all_songs->songs, 0), "bad song");
  test_passed();
}

void test_cursor_patch(patchmaster *pm) {
  test_start(pm);
  cursor *c = pm->cursor;
  song *s = cursor_song(c);
  tassert(cursor_patch(c) == list_at(s->patches, 0), "bad patch");
  test_passed();
}

void test_cursor(patchmaster *pm) {
  test_cursor_init_empty(pm);
  test_cursor_init(pm);
  test_cursor_next_patch(pm);
  test_cursor_next_patch_at_end_of_song(pm);
  test_cursor_next_patch_at_end_of_song_list(pm);
  test_cursor_prev_patch(pm);
  test_cursor_prev_patch_start_of_song(pm);
  test_cursor_prev_patch_start_of_song_list(pm);
  test_cursor_next_song(pm);
  test_cursor_prev_song(pm);
  test_cursor_song_list(pm);
  test_cursor_song(pm);
  test_cursor_patch(pm);
}
