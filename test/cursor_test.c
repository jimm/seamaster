#include "seamaster_test.h"
#include "../src/cursor.h"
#include "cursor_test.h"

const char *BAD_SONG_LIST = "bad song list";
const char *BAD_SONG = "bad song";
const char *BAD_PATCH = "bad patch";

void test_cursor_init_empty(patchmaster *_) {
  patchmaster *pm = patchmaster_new();
  cursor *c = pm->cursor;
  cursor_init(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == -1, BAD_SONG);
  tassert(c->patch_index == -1, BAD_PATCH);
}

void test_cursor_init(patchmaster *pm) {
  cursor *c = pm->cursor;
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_patch(patchmaster *pm) {
  cursor *c = pm->cursor;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
}

void test_cursor_next_patch_at_end_of_song(patchmaster *pm) {
  cursor *c = pm->cursor;
  c->patch_index = 1;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_patch_at_end_of_song_list(patchmaster *pm) {
  cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  cursor_next_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
}

void test_cursor_prev_patch(patchmaster *pm) {
  cursor *c = pm->cursor;
  c->patch_index = 1;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_patch_start_of_song(patchmaster *pm) {
  cursor *c = pm->cursor;
  c->song_index = 1;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_patch_start_of_song_list(patchmaster *pm) {
  cursor *c = pm->cursor;
  cursor_prev_patch(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_song(patchmaster *pm) {
  cursor *c = pm->cursor;
  cursor_next_song(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_song(patchmaster *pm) {
  cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  cursor_prev_song(c);
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_song_list(patchmaster *pm) {
  cursor *c = pm->cursor;
  tassert(cursor_song_list(c) == pm->all_songs, BAD_SONG_LIST);
}

void test_cursor_song(patchmaster *pm) {
  cursor *c = pm->cursor;
  tassert(cursor_song(c) == list_at(pm->all_songs->songs, 0), BAD_SONG);
}

void test_cursor_patch(patchmaster *pm) {
  cursor *c = pm->cursor;
  song *s = cursor_song(c);
  tassert(cursor_patch(c) == list_at(s->patches, 0), BAD_PATCH);
}

void test_cursor(patchmaster *pm) {
  test_run(test_cursor_init_empty);
  test_run(test_cursor_init);
  test_run(test_cursor_next_patch);
  test_run(test_cursor_next_patch_at_end_of_song);
  test_run(test_cursor_next_patch_at_end_of_song_list);
  test_run(test_cursor_prev_patch);
  test_run(test_cursor_prev_patch_start_of_song);
  test_run(test_cursor_prev_patch_start_of_song_list);
  test_run(test_cursor_next_song);
  test_run(test_cursor_prev_song);
  test_run(test_cursor_song_list);
  test_run(test_cursor_song);
  test_run(test_cursor_patch);
}
