#include "seamaster_test.h"
#include "../src/cursor.h"
#include "cursor_test.h"

const char *BAD_SONG_LIST = "bad song list";
const char *BAD_SONG = "bad song";
const char *BAD_PATCH = "bad patch";

void test_cursor_init_empty(patchmaster *_) {
  patchmaster *pm = patchmaster_new();
  Cursor *c = pm->cursor;
  c->init();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == -1, BAD_SONG);
  tassert(c->patch_index == -1, BAD_PATCH);
}

void test_cursor_init(patchmaster *pm) {
  Cursor *c = pm->cursor;
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_patch(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
}

void test_cursor_next_patch_at_end_of_song(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_patch_at_end_of_song_list(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
}

void test_cursor_prev_patch(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_patch_start_of_song(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_patch_start_of_song_list(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_next_song(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->next_song();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_prev_song(patchmaster *pm) {
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->prev_song();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
}

void test_cursor_song_list(patchmaster *pm) {
  Cursor *c = pm->cursor;
  tassert(c->song_list() == pm->all_songs, BAD_SONG_LIST);
}

void test_cursor_song(patchmaster *pm) {
  Cursor *c = pm->cursor;
  tassert(c->song() == list_first(pm->all_songs->songs), BAD_SONG);
}

void test_cursor_patch(patchmaster *pm) {
  Cursor *c = pm->cursor;
  Song *s = c->song();
  tassert(c->patch() == list_first(s->patches), BAD_PATCH);
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
