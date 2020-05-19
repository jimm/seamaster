#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/seamaster.h"

#define CATCH_CATEGORY "[seamaster]"
#include "test_helper.h"

void assert_no_start_sent(SeaMaster *sm) {
  for (auto& out : sm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        REQUIRE("assert no start sent" == "start was sent");
  }
  REQUIRE(true);
}

void assert_no_stop_sent(SeaMaster *sm) {
  for (auto& out : sm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
        REQUIRE("assert no stop sent" == "stop was sent");
  }
  REQUIRE(true);
}

void assert_start_sent(SeaMaster *sm) {
  for (auto& out : sm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        return;
  }
}

void assert_stop_sent(SeaMaster *sm) {
  Output *out = sm->outputs[0];
  REQUIRE(!out->real_port());
  for (int i = 0; i < out->num_io_messages; ++i)
    if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
      return;
  REQUIRE("assert stop sent" == "stop message not sent");
}

void clear_out_io_messages(SeaMaster *sm) {
  for (auto& out : sm->outputs)
    out->num_io_messages = 0;
}

TEST_CASE("send start and stop messages", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  sm->start();                  // Another Song
  assert_no_stop_sent(sm);
  assert_no_start_sent(sm);

  clear_out_io_messages(sm);
  sm->next_patch();             // second patch in song: has start and stop
  REQUIRE(sm->cursor->patch()->start_message->messages.size() > 0);
  assert_no_stop_sent(sm);
  assert_start_sent(sm);

  clear_out_io_messages(sm);
  sm->next_song();              // To Each His Own
  assert_stop_sent(sm);
  assert_no_start_sent(sm);

  clear_out_io_messages(sm);
  sm->prev_song();              // Another Song
  assert_no_stop_sent(sm);
  assert_no_start_sent(sm);

  sm->stop();
}

TEST_CASE("all songs sorted", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  REQUIRE(sm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(sm->all_songs->songs[1]->name == "Song Without Explicit Patch");
  REQUIRE(sm->all_songs->songs[2]->name == "To Each His Own");
}

TEST_CASE("inserted song sorts properly", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Song *s = new Song(UNDEFINED_ID, "Bees, Bees!");
  sm->all_songs->songs.push_back(s);
  sm->sort_all_songs();

  REQUIRE(sm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(sm->all_songs->songs[1]->name == "Bees, Bees!");
  REQUIRE(sm->all_songs->songs[2]->name == "Song Without Explicit Patch");
  REQUIRE(sm->all_songs->songs[3]->name == "To Each His Own");
}

TEST_CASE("inserted song sorts properly, case-sensitively", CATCH_CATEGORY) {
  SeaMaster *sm = load_test_data();
  Song *s = new Song(UNDEFINED_ID, "a jar full of bees");
  sm->all_songs->songs.push_back(s);
  sm->sort_all_songs();

  REQUIRE(sm->all_songs->songs[0]->name == "Another Song");
  REQUIRE(sm->all_songs->songs[1]->name == "Song Without Explicit Patch");
  REQUIRE(sm->all_songs->songs[2]->name == "To Each His Own");
  REQUIRE(sm->all_songs->songs[3]->name == "a jar full of bees");
}
