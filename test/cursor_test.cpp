#include "test_helper.h"
#include "../src/load.h"
#include "cursor_test.h"

#define TEST_FILE "test/testfile.sm"

const char *BAD_SONG_LIST = "bad song list";
const char *BAD_SONG = "bad song";
const char *BAD_PATCH = "bad patch";

PatchMaster *cursor_pm() {
  PatchMaster *pm = create_pm();
  load(pm, TEST_FILE);
  pm->cursor->init();
  return pm;
}

void test_cursor_init_empty() {
  PatchMaster *pm = new PatchMaster();
  Cursor *c = pm->cursor;
  c->init();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == -1, BAD_SONG);
  tassert(c->patch_index == -1, BAD_PATCH);
}

void test_cursor_init() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_next_patch() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
  delete pm;
}

void test_cursor_next_patch_at_end_of_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_next_patch_at_end_of_song_list() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 1, BAD_PATCH);
  delete pm;
}

void test_cursor_prev_patch() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_prev_patch_start_of_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_prev_patch_start_of_song_list() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->prev_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_next_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->next_song();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 1, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_prev_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->prev_song();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 0, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
  delete pm;
}

void test_cursor_song_list() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  tassert(c->song_list() == pm->all_songs, BAD_SONG_LIST);
  delete pm;
}

void test_cursor_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  tassert(c->song() == list_first(pm->all_songs->songs), BAD_SONG);
  delete pm;
}

void test_cursor_patch() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  Song *s = c->song();
  tassert(c->patch() == list_first(s->patches), BAD_PATCH);
  delete pm;
}

void test_cursor() {
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
