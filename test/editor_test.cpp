#include "catch.hpp"
#include "test_helper.h"
#include "../src/editor.h"

#define CATCH_CATEGORY "[editor]"
#define TEST_FILE "test/testfile.org"

TEST_CASE("create message", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  e.create_message();
  REQUIRE(pm->messages.back()->name == "Unnamed Message");
}

TEST_CASE("create trigger", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);
  Input *input = pm->inputs.front();

  int num_triggers = input->triggers.size();
  e.create_trigger(pm->inputs.front());
  REQUIRE(input->triggers.size() == num_triggers + 1);
}

TEST_CASE("create song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  e.create_song();
  Song *s = pm->all_songs->songs.back();
  REQUIRE(s->name == "Unnamed Song");
  REQUIRE(s->patches.size() == 1);
  REQUIRE(s->patches.front()->name == "Unnamed Patch");
}

TEST_CASE("create patch", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  int num_patches = pm->cursor->song()->patches.size();
  e.create_patch();
  REQUIRE(pm->cursor->song()->patches.size() == num_patches + 1);
  REQUIRE(pm->cursor->song()->patches.back()->name == "Unnamed Patch");
}

TEST_CASE("create connection", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  Patch *patch = pm->cursor->patch();
  int num_conns = patch->connections.size();
  e.create_connection(patch, pm->inputs.front(), pm->outputs.front());
  REQUIRE(patch->connections.size() == num_conns + 1);
}

TEST_CASE("create set list", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  e.create_set_list();
  REQUIRE(pm->set_lists.back()->name == "Unnamed Set List");
}

TEST_CASE("destroy message", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  int num_messages = pm->messages.size();
  e.destroy_message(pm->messages.front());
  REQUIRE(pm->messages.size() == num_messages - 1);
}

TEST_CASE("destroy trigger", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);
  Input *input = pm->inputs.front();

  int num_triggers = input->triggers.size();
  REQUIRE(num_triggers > 0);    // test sanity check
  e.destroy_trigger(input->triggers.front());
  REQUIRE(input->triggers.size() == num_triggers - 1);
}

TEST_CASE("destroy song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  int num_songs = pm->all_songs->songs.size();
  e.destroy_song(pm->all_songs->songs.back());
  REQUIRE(pm->all_songs->songs.size() == num_songs - 1);
  REQUIRE(pm->set_lists[1]->songs.size() == 1);
  REQUIRE(pm->set_lists[2]->songs.size() == 1);
}

TEST_CASE("create and destroy song", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

  e.create_song();
  e.destroy_song(pm->all_songs->songs.back());
}

TEST_CASE("destroy patch", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

}

TEST_CASE("destroy connection", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

}

TEST_CASE("destroy set list", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->cursor->init();
  Editor e(pm);

}
