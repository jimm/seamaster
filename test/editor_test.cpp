#include "catch.hpp"
#include "test_helper.h"
#include "../src/editor.h"

#define CATCH_CATEGORY "[editor]"
#define TEST_FILE "test/testfile.org"

TEST_CASE("create message", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  e.create_message();
  REQUIRE(pm->messages.back()->name == "Unnamed Message");
}

TEST_CASE("create trigger", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);
  Input *input = pm->inputs.front();

  int num_triggers = input->triggers.size();
  e.create_trigger(pm->inputs.front());
  REQUIRE(input->triggers.size() == num_triggers + 1);
}

TEST_CASE("create song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  e.create_song();
  Song *s = pm->all_songs->songs.back();
  REQUIRE(s->name == "Unnamed Song");
  REQUIRE(s->patches.size() == 1);
  REQUIRE(s->patches.front()->name == "Unnamed Patch");
}

TEST_CASE("create patch", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  Cursor *c = pm->cursor;
  c->init();
  Editor e(pm);

  int num_patches = c->song()->patches.size();
  e.create_patch();
  REQUIRE(c->song()->patches.size() == num_patches + 1);
  REQUIRE(c->song()->patches.back()->name == "Unnamed Patch");
}

TEST_CASE("create connection", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  Cursor *c = pm->cursor;
  c->init();
  Editor e(pm);

  Patch *patch = c->patch();
  int num_conns = patch->connections.size();
  e.create_connection(patch, pm->inputs.front(), pm->outputs.front());
  REQUIRE(patch->connections.size() == num_conns + 1);
}

TEST_CASE("create set list", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  e.create_set_list();
  REQUIRE(pm->set_lists.back()->name == "Unnamed Set List");
}

TEST_CASE("destroy message", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  int num_messages = pm->messages.size();
  e.destroy_message(pm->messages.front());
  REQUIRE(pm->messages.size() == num_messages - 1);
}

TEST_CASE("destroy trigger", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);
  Input *input = pm->inputs.front();

  int num_triggers = input->triggers.size();
  REQUIRE(num_triggers > 0);    // test sanity check
  e.destroy_trigger(input->triggers.front());
  REQUIRE(input->triggers.size() == num_triggers - 1);
}

TEST_CASE("destroy song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  int num_songs = pm->all_songs->songs.size();
  e.destroy_song(pm->all_songs->songs.back());
  REQUIRE(pm->all_songs->songs.size() == num_songs - 1);
  REQUIRE(pm->set_lists[1]->songs.size() == 1);
  REQUIRE(pm->set_lists[2]->songs.size() == 1);
}

TEST_CASE("create and destroy song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();
  Editor e(pm);

  e.create_song();
  e.destroy_song(pm->all_songs->songs.back());
}

TEST_CASE("destroy first patch in song with mult. patches", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  Cursor *c = pm->cursor;
  c->init();
  Patch *old_patch = c->patch();
  int num_patches = c->song()->patches.size();

  // sanity check
  REQUIRE(c->song() == pm->all_songs->songs[0]);
  REQUIRE(c->patch() == c->song()->patches[0]);

  Editor e(pm);
  e.destroy_patch(c->song(), c->patch());

  // old_patch has been deallocated, but we can still check that the current
  // cursor patch is not the same value.
  REQUIRE(c->song() == pm->all_songs->songs[0]);
  REQUIRE(c->song()->patches.size() == num_patches - 1);

  // current patch is the first patch in the song, but not the same as
  // old_patch
  REQUIRE(c->patch() == c->song()->patches[0]);
  REQUIRE(c->patch() != old_patch);
}

TEST_CASE("destroy last patch in song with mult. patches", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  Cursor *c = pm->cursor;
  c->init();

  int num_patches = c->song()->patches.size();
  c->patch_index = num_patches - 1;
  Patch *old_patch = c->patch();

  // sanity check
  REQUIRE(c->song() == pm->all_songs->songs[0]);
  REQUIRE(c->patch() == c->song()->patches.back());
  REQUIRE(c->song()->patches.size() == num_patches);

  Editor e(pm);
  e.destroy_patch(c->song(), c->patch());

  // old_patch has been deallocated, but we can still check that the current
  // cursor patch is not the same value.
  REQUIRE(c->song() == pm->all_songs->songs[0]);
  REQUIRE(c->song()->patches.size() == num_patches - 1);

  // current patch is the first patch in the song, but not the same as
  // old_patch
  REQUIRE(c->patch() == c->song()->patches.back());
  REQUIRE(c->patch() != old_patch);
}

TEST_CASE("destroy connection", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();

  Patch *p = pm->cursor->patch();
  Connection *old_conn = p->connections.back();
  int num_conns = p->connections.size();

  Editor e(pm);
  e.destroy_connection(p, old_conn);
  REQUIRE(p->connections.size() == num_conns - 1);
  for (auto &conn : p->connections)
    if (conn == old_conn)
      FAIL("old_conn was not removed");
}

TEST_CASE("destroy set list", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_data();
  pm->cursor->init();

  SetList *old_set_list = pm->set_lists.back();
  int num_set_lists = pm->set_lists.size();

  Editor e(pm);
  e.destroy_set_list(old_set_list);

  REQUIRE(pm->set_lists.size() == num_set_lists - 1);
  for (auto &slist : pm->set_lists)
    if (slist == old_set_list)
      FAIL("old_set_list was not removed");
}
