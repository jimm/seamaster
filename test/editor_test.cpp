#include "catch.hpp"
#include "test_helper.h"
#include "../src/editor.h"

#define CATCH_CATEGORY "[editor]"
#define TEST_FILE "test/testfile.org"

TEST_CASE("create and add message", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  Message *m = e.create_message();
  e.add_message(m);
  REQUIRE(sm->messages.back() == m);
}

TEST_CASE("create and add trigger", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);
  Input *input = sm->inputs.front();

  Trigger *t = e.create_trigger(input);
  e.add_trigger(t);
  REQUIRE(sm->triggers.back() == t);
}

TEST_CASE("create song", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  Song *created_song = e.create_song();
  REQUIRE(created_song->name == "Unnamed Song");
  REQUIRE(created_song->patches.size() == 1);
  REQUIRE(created_song->patches.front()->name == "Unnamed Patch");
}

TEST_CASE("add song inserts into current song list", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  Song *created_song = e.create_song();
  e.add_song(created_song);
  // It'll be last because name is "Unnamed Song" and that's last
  // alphabetically
  REQUIRE(created_song == sm->all_songs->songs.back());
}

TEST_CASE("add song inserts into empty song list", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  SetList *set_list = e.create_set_list();
  e.add_set_list(set_list);
  sm->cursor->set_list_index = sm->set_lists.size() - 1;
  // sanity checks
  REQUIRE(set_list == sm->cursor->set_list());
  REQUIRE(set_list->songs.size() == 0);

  Song *created_song = e.create_song();
  e.add_song(created_song);
  REQUIRE(set_list->songs.size() == 1);
  REQUIRE(created_song == set_list->songs.front());
}

TEST_CASE("create and add patch", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Cursor *c = sm->cursor;
  c->init();
  Editor e(sm);

  int num_patches = c->song()->patches.size();
  Patch *p = e.create_patch();
  e.add_patch(p, c->song());
  REQUIRE(c->song()->patches.size() == num_patches + 1);
  REQUIRE(c->song()->patches.back() == p);
}

TEST_CASE("create and add connection", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Cursor *c = sm->cursor;
  c->init();
  Editor e(sm);

  Patch *patch = c->patch();
  int num_conns = patch->connections.size();
  Connection *conn = e.create_connection(sm->inputs.front(), sm->outputs.front());
  e.add_connection(conn, patch);
  REQUIRE(patch->connections.size() == num_conns + 1);
  REQUIRE(patch->connections.back() == conn);
}

TEST_CASE("create set list", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  SetList *slist = e.create_set_list();
  e.add_set_list(slist);
  REQUIRE(sm->set_lists.back() == slist);
}

TEST_CASE("destroy message", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  int num_messages = sm->messages.size();
  e.destroy_message(sm->messages.front());
  REQUIRE(sm->messages.size() == num_messages - 1);
}

TEST_CASE("destroy trigger", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);
  Input *input = sm->inputs.front();

  int num_triggers = input->triggers.size();
  REQUIRE(num_triggers > 0);    // test sanity check
  e.destroy_trigger(input->triggers.front());
  REQUIRE(input->triggers.size() == num_triggers - 1);
}

TEST_CASE("destroy song", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  int num_songs = sm->all_songs->songs.size();
  e.destroy_song(sm->all_songs->songs.back());
  REQUIRE(sm->all_songs->songs.size() == num_songs - 1);
  REQUIRE(sm->set_lists[1]->songs.size() == 1);
  REQUIRE(sm->set_lists[2]->songs.size() == 1);
}

TEST_CASE("create and destroy song", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();
  Editor e(sm);

  e.create_song();
  e.destroy_song(sm->all_songs->songs.back());
}

TEST_CASE("destroy first patch in song with mult. patches", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Cursor *c = sm->cursor;
  c->init();
  Patch *old_patch = c->patch();
  int num_patches = c->song()->patches.size();

  // sanity check
  REQUIRE(c->song() == sm->all_songs->songs[0]);
  REQUIRE(c->patch() == c->song()->patches[0]);

  Editor e(sm);
  e.destroy_patch(c->song(), c->patch());

  // old_patch has been deallocated, but we can still check that the current
  // cursor patch is not the same value.
  REQUIRE(c->song() == sm->all_songs->songs[0]);
  REQUIRE(c->song()->patches.size() == num_patches - 1);

  // current patch is the first patch in the song, but not the same as
  // old_patch
  REQUIRE(c->patch() == c->song()->patches[0]);
  REQUIRE(c->patch() != old_patch);
}

TEST_CASE("destroy last patch in song with mult. patches", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Cursor *c = sm->cursor;
  c->init();

  int num_patches = c->song()->patches.size();
  c->patch_index = num_patches - 1;
  Patch *old_patch = c->patch();

  // sanity check
  REQUIRE(c->song() == sm->all_songs->songs[0]);
  REQUIRE(c->patch() == c->song()->patches.back());
  REQUIRE(c->song()->patches.size() == num_patches);

  Editor e(sm);
  e.destroy_patch(c->song(), c->patch());

  // old_patch has been deallocated, but we can still check that the current
  // cursor patch is not the same value.
  REQUIRE(c->song() == sm->all_songs->songs[0]);
  REQUIRE(c->song()->patches.size() == num_patches - 1);

  // current patch is the first patch in the song, but not the same as
  // old_patch
  REQUIRE(c->patch() == c->song()->patches.back());
  REQUIRE(c->patch() != old_patch);
}

TEST_CASE("destroy connection", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();

  Patch *p = sm->cursor->patch();
  Connection *old_conn = p->connections.back();
  int num_conns = p->connections.size();

  Editor e(sm);
  e.destroy_connection(p, old_conn);
  REQUIRE(p->connections.size() == num_conns - 1);
  for (auto &conn : p->connections)
    if (conn == old_conn)
      FAIL("old_conn was not removed");
}

TEST_CASE("destroy set list", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->cursor->init();

  SetList *old_set_list = sm->set_lists.back();
  int num_set_lists = sm->set_lists.size();

  Editor e(sm);
  e.destroy_set_list(old_set_list);

  REQUIRE(sm->set_lists.size() == num_set_lists - 1);
  for (auto &slist : sm->set_lists)
    if (slist == old_set_list)
      FAIL("old_set_list was not removed");
}

TEST_CASE("add then destroy patches", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Cursor *c = sm->cursor;
  c->init();

  Song *song = c->song();
  REQUIRE(song->patches.size() == 2); // sanity check

  Editor e;
  Patch *created1 = e.create_patch();
  Patch *created2 = e.create_patch();
  e.add_patch(created1, song);
  e.add_patch(created2, song);

  REQUIRE(created1 != created2);
  REQUIRE(created2 == song->patches.back());

  REQUIRE(song->patches.size() == 4);
  REQUIRE(c->patch() == song->patches.front());

  c->patch_index = 2;              // first created patch
  REQUIRE(c->patch() == created1); // sanity check

  e.destroy_patch(song, c->patch());
  REQUIRE(c->patch_index == 2);
  REQUIRE(c->patch() == created2);

  e.destroy_patch(song, c->patch());
  REQUIRE(c->song() == song);
  REQUIRE(c->patch_index == 1);
}
