#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/seamaster.h"

#define CATCH_CATEGORY "[seamaster]"
#include "test_helper.h"

void assert_no_start_sent(SeaMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        REQUIRE("assert no start sent" == "start was sent");
  }
  REQUIRE(true);
}

void assert_no_stop_sent(SeaMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
        REQUIRE("assert no stop sent" == "stop was sent");
  }
  REQUIRE(true);
}

void assert_start_sent(SeaMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        return;
  }
}

void assert_stop_sent(SeaMaster *pm) {
  Output *out = pm->outputs[0];
  REQUIRE(!out->real_port());
  for (int i = 0; i < out->num_io_messages; ++i)
    if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
      return;
  REQUIRE("assert stop sent" == "stop message not sent");
}

void clear_out_io_messages(SeaMaster *pm) {
  for (auto& out : pm->outputs)
    out->num_io_messages = 0;
}

TEST_CASE("send start and stop messages", CATCH_CATEGORY) {
  SeaMaster *pm = load_test_data();
  pm->start();                  // Another Song
  assert_no_stop_sent(pm);
  assert_no_start_sent(pm);

  clear_out_io_messages(pm);
  pm->next_patch();             // second patch in song: has start and stop
  REQUIRE(pm->cursor->patch()->start_message->messages.size() > 0);
  assert_no_stop_sent(pm);
  assert_start_sent(pm);

  clear_out_io_messages(pm);
  pm->next_song();              // To Each His Own
  assert_stop_sent(pm);
  assert_no_start_sent(pm);

  clear_out_io_messages(pm);
  pm->prev_song();              // Another Song
  assert_no_stop_sent(pm);
  assert_no_start_sent(pm);

  pm->stop();
}

TEST_CASE("all songs sorted", CATCH_CATEGORY) {
  SeaMaster *pm = load_test_data();
  REQUIRE(pm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(pm->all_songs->songs[1]->name == "Song Without Explicit Patch");
  REQUIRE(pm->all_songs->songs[2]->name == "To Each His Own");
}

TEST_CASE("inserted song sorts properly", CATCH_CATEGORY) {
  SeaMaster *pm = load_test_data();
  Song *s = new Song(UNDEFINED_ID, "Bees, Bees!");
  pm->all_songs->songs.push_back(s);
  pm->sort_all_songs();

  REQUIRE(pm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(pm->all_songs->songs[1]->name == "Bees, Bees!");
  REQUIRE(pm->all_songs->songs[2]->name == "Song Without Explicit Patch");
  REQUIRE(pm->all_songs->songs[3]->name == "To Each His Own");
}

TEST_CASE("inserted song sorts properly, case-sensitively", CATCH_CATEGORY) {
  SeaMaster *pm = load_test_data();
  Song *s = new Song(UNDEFINED_ID, "a jar full of bees");
  pm->all_songs->songs.push_back(s);
  pm->sort_all_songs();

  REQUIRE(pm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(pm->all_songs->songs[1]->name == "Song Without Explicit Patch");
  REQUIRE(pm->all_songs->songs[2]->name == "To Each His Own");
  REQUIRE(pm->all_songs->songs[3]->name == "a jar full of bees");
}
