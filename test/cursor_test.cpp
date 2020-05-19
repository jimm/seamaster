#include "catch.hpp"
#include "test_helper.h"
#include "../src/storage.h"

#define CATCH_CATEGORY "[cursor]"
#define TEST_FILE "test/testfile.org"

SeaMaster *cursor_sm() {
  SeaMaster *sm = load_test_data();
  sm->testing = true;
  sm->cursor->init();
  return sm;
}

// ================ initialization

TEST_CASE("init empty", CATCH_CATEGORY) {
  SeaMaster *sm = new SeaMaster();
  Cursor *c = sm->cursor;
  c->init();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == -1);
  REQUIRE(c->patch_index == -1);
}

TEST_CASE("init", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm(); // calls Cursor::init
  Cursor *c = sm->cursor;
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

// ================ movement

TEST_CASE("next patch", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->next_patch();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 1);
  delete sm;
}

TEST_CASE("next patch at end of song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->patch_index = 1;
  c->next_patch();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

TEST_CASE("next patch at end of set list", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->set_list_index = 1;        // Set List One
  c->song_index = 1;            // Another Song
  c->patch_index = 1;           // Split Into Two Outputs
  c->next_patch();
  REQUIRE(c->set_list_index == 1);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 1);
  delete sm;
}

TEST_CASE("prev patch", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->patch_index = 1;
  c->prev_patch();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

TEST_CASE("prev patch start of song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = 1;
  c->prev_patch();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

TEST_CASE("prev patch start of set list", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->prev_patch();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

TEST_CASE("next song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->next_song();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 1);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

TEST_CASE("prev song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  c->prev_song();
  REQUIRE(c->set_list_index == 0);
  REQUIRE(c->song_index == 0);
  REQUIRE(c->patch_index == 0);
  delete sm;
}

// ================ has_{next,prev}_{song,patch} predicates

TEST_CASE("has next song true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(c->has_next_song());
}

TEST_CASE("has next song false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = c->set_list()->songs.size() - 1;
  REQUIRE(!c->has_next_song());
}

TEST_CASE("has prev song true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = 1;
  REQUIRE(c->has_prev_song());
}

TEST_CASE("has prev song false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(!c->has_prev_song());
}

TEST_CASE("has next patch true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(c->has_next_patch());
}

TEST_CASE("has next patch false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = c->set_list()->songs.size() - 1;
  c->patch_index = c->song()->patches.size() - 1;
  REQUIRE(!c->has_next_patch());
}

TEST_CASE("has prev patch true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->patch_index = 1;
  REQUIRE(c->has_prev_patch());
}

TEST_CASE("has prev patch false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(!c->has_prev_patch());
}

TEST_CASE("has next patch in song true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(c->has_next_patch_in_song());
}

TEST_CASE("has next patch in song false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = c->set_list()->songs.size() - 1;
  c->patch_index = c->song()->patches.size() - 1;
  REQUIRE(!c->has_next_patch_in_song());
}

TEST_CASE("has prev patch in song true", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = 1;
  c->patch_index = 1;
  REQUIRE(c->has_prev_patch_in_song());
}

TEST_CASE("has prev patch in song false", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  c->song_index = 1;
  REQUIRE(!c->has_prev_patch_in_song());
}

// ================ defaults

TEST_CASE("default set list is all songs", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(c->set_list() == sm->all_songs);
  delete sm;
}

TEST_CASE("song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  REQUIRE(c->song() == sm->all_songs->songs[0]);
  delete sm;
}

TEST_CASE("patch", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;
  Song *s = c->song();
  REQUIRE(c->patch() == s->patches[0]);
  delete sm;
}

// ================ goto

TEST_CASE("goto song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;

  c->goto_song("nother");
  Song *s = c->song();
  REQUIRE(s != nullptr);
  REQUIRE(s->name == "Another Song");

  delete sm;
}

TEST_CASE("goto song no such song", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;

  Song *before = c->song();
  REQUIRE(before != nullptr);

  c->goto_song("nosuch");
  Song *s = c->song();
  REQUIRE(s == before);

  delete sm;
}

TEST_CASE("goto set list", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;

  c->goto_set_list("two");
  SetList *sl = c->set_list();
  REQUIRE(sl != nullptr);
  REQUIRE(sl->name == "Set List Two");

  delete sm;
}

TEST_CASE("goto set list no such set list", CATCH_CATEGORY) {
  SeaMaster *sm = cursor_sm();
  Cursor *c = sm->cursor;

  SetList *before = c->set_list();
  REQUIRE(before != nullptr);

  c->goto_set_list("nosuch");
  SetList *sl = c->set_list();
  REQUIRE(sl == before);

  delete sm;
}
