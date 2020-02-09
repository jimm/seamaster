#include "catch.hpp"
#include "../src/patchmaster.h"

#define CATCH_CATEGORY "[patchmaster]"
#include "test_helper.h"

void assert_no_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        REQUIRE("assert no start sent" == "start was sent");
  }
  REQUIRE(true);
}

void assert_no_stop_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
        REQUIRE("assert no stop sent" == "stop was sent");
  }
  REQUIRE(true);
}

void assert_start_sent(PatchMaster *pm) {
  for (auto& out : pm->outputs) {
    for (int i = 0; i < out->num_io_messages; ++i)
      if (out->io_messages[i] == Pm_Message(0xb0, 7, 0x7f))
        return;
  }
}

void assert_stop_sent(PatchMaster *pm) {
  Output *out = pm->outputs[0];
  REQUIRE(!out->real_port());
  for (int i = 0; i < out->num_io_messages; ++i)
    if (out->io_messages[i] == Pm_Message(0xb2, 7, 0x7f))
      return;
  REQUIRE("assert stop sent" == "stop message not sent");
}

void clear_out_io_messages(PatchMaster *pm) {
  for (auto& out : pm->outputs)
    out->num_io_messages = 0;
}

TEST_CASE("send start and stop messages", CATCH_CATEGORY) {
  PatchMaster *pm = load_test_file();
  pm->start();                  // Another Song
  assert_no_stop_sent(pm);
  assert_no_start_sent(pm);

  clear_out_io_messages(pm);
  pm->next_patch();             // second patch in song: has start and stop
  REQUIRE(pm->cursor->patch()->start_messages.size() > 0);
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
