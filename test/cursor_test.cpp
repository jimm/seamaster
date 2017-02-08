#include "test_helper.h"
#include "../src/loader.h"
#include "cursor_test.h"

#define TEST_FILE "test/testfile.org"

const char *BAD_SONG_LIST = "bad song list";
const char *BAD_SONG = "bad song";
const char *BAD_PATCH = "bad patch";

PatchMaster *cursor_pm() {
  PatchMaster *pm = create_pm();
  Loader loader(*pm);
  loader.load(TEST_FILE);
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
  c->song_index = 2;
  c->patch_index = 0;
  c->next_patch();
  tassert(c->song_list_index == 0, BAD_SONG_LIST);
  tassert(c->song_index == 2, BAD_SONG);
  tassert(c->patch_index == 0, BAD_PATCH);
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
  tassert(c->song() == pm->all_songs->songs.first(), BAD_SONG);
  delete pm;
}

void test_cursor_patch() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  Song *s = c->song();
  tassert(c->patch() == s->patches.first(), BAD_PATCH);
  delete pm;
}

void test_cursor_goto_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  c->goto_song("nother");
  Song *s = c->song();
  tassert(s != 0, 0);
  tassert(s->name == "Another Song", 0);

  delete pm;
}

void test_cursor_goto_song_no_such_song() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  Song *before = c->song();
  tassert(before != 0, 0);

  c->goto_song("nosuch");
  Song *s = c->song();
  tassert(s == before, 0);

  delete pm;
}

void test_cursor_goto_song_list() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  c->goto_song_list("two");
  SongList *sl = c->song_list();
  tassert(sl != 0, 0);
  tassert(sl->name == "Song List Two", 0);

  delete pm;
}

void test_cursor_goto_song_list_no_such_song_list() {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  SongList *before = c->song_list();
  tassert(before != 0, 0);

  c->goto_song_list("nosuch");
  SongList *sl = c->song_list();
  tassert(sl == before, 0);

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
  test_run(test_cursor_goto_song);
  test_run(test_cursor_goto_song_no_such_song);
  test_run(test_cursor_goto_song_list);
  test_run(test_cursor_goto_song_list_no_such_song_list);
}
