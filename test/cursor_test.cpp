#include "catch.hpp"
#include "test_helper.h"
#include "../src/loader.h"

#define CATCH_CATEGORY "[cursor]"
#define TEST_FILE "test/testfile.org"

PatchMaster *cursor_pm() {
  Loader loader;
  PatchMaster *pm = loader.load(TEST_FILE, true);
  pm->testing = true;
  pm->cursor->init();
  return pm;
}

TEST_CASE("init empty", CATCH_CATEGORY) {
  PatchMaster *pm = new PatchMaster();
  Cursor *c = pm->cursor;
  c->init();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == -1);
  REQUIRE(c->patch_index == -1);
}

TEST_CASE("init", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("next patch", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->next_patch();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 1);
  delete pm;
}

TEST_CASE("next patch at end of song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->next_patch();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("next patch at end of song list", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_list_index = 1;       // Song List One
  c->song_index = 1;            // Another Song
  c->patch_index = 1;           // Split Into Two Outputs
  c->next_patch();
  REQUIRE(c->song_list_index == 1);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 1);
  delete pm;
}

TEST_CASE("prev patch", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->patch_index = 1;
  c->prev_patch();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("prev patch start of song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->prev_patch();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("prev patch start of song list", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->prev_patch();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("next song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->next_song();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("prev song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->prev_song();
  REQUIRE(c->song_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete pm;
}

TEST_CASE("default song list is all songs", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  REQUIRE(c->song_list() == pm->all_songs);
  delete pm;
}

TEST_CASE("song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  REQUIRE(c->song() == pm->all_songs->songs[0]);
  delete pm;
}

TEST_CASE("patch", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;
  Song *s = c->song();
  REQUIRE(c->patch() == s->patches[0]);
  delete pm;
}

TEST_CASE("goto song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  c->goto_song("nother");
  Song *s = c->song();
  REQUIRE(s != nullptr);
  REQUIRE(s->name == "Another Song");

  delete pm;
}

TEST_CASE("goto song no such song", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  Song *before = c->song();
  REQUIRE(before != nullptr);

  c->goto_song("nosuch");
  Song *s = c->song();
  REQUIRE(s == before);

  delete pm;
}

TEST_CASE("goto song list", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  c->goto_song_list("two");
  SongList *sl = c->song_list();
  REQUIRE(sl != nullptr);
  REQUIRE(sl->name == "Song List Two");

  delete pm;
}

TEST_CASE("goto song list no such song list", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  SongList *before = c->song_list();
  REQUIRE(before != nullptr);

  c->goto_song_list("nosuch");
  SongList *sl = c->song_list();
  REQUIRE(sl == before);

  delete pm;
}

TEST_CASE("attempt goto", CATCH_CATEGORY) {
  PatchMaster *pm = cursor_pm();
  Cursor *c = pm->cursor;

  Cursor target(pm);
  target.song_list_index = 1;
  target.song_index = 1;
  target.patch_index = 1;

  c->attempt_goto(&target);
  REQUIRE(c->song_list_index == 1);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 1);
}
